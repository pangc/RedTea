#include "d3d12-backend.h"

#include "../../RHI/misc.h"
#include <sstream>

namespace redtea
{
namespace device
{
	void CommandList::setResourceStatesForBindingSet(IBindingSet* _bindingSet)
	{
		if (_bindingSet->getDesc() == nullptr)
			return; // is bindless

		BindingSet* bindingSet = CHECKED_CAST<BindingSet*>(_bindingSet);

		for (auto bindingIndex : bindingSet->bindingsThatNeedTransitions)
		{
			const BindingSetItem& binding = bindingSet->desc.bindings[bindingIndex];

			switch (binding.type)  // NOLINT(clang-diagnostic-switch-enum)
			{
			case ResourceType::Texture_SRV:
				requireTextureState(CHECKED_CAST<ITexture*>(binding.resourceHandle), binding.subresources, ResourceStates::ShaderResource);
				break;

			case ResourceType::Texture_UAV:
				requireTextureState(CHECKED_CAST<ITexture*>(binding.resourceHandle), binding.subresources, ResourceStates::UnorderedAccess);
				break;

			case ResourceType::TypedBuffer_SRV:
			case ResourceType::StructuredBuffer_SRV:
				requireBufferState(CHECKED_CAST<IBuffer*>(binding.resourceHandle), ResourceStates::ShaderResource);
				break;

			case ResourceType::TypedBuffer_UAV:
			case ResourceType::StructuredBuffer_UAV:
				requireBufferState(CHECKED_CAST<IBuffer*>(binding.resourceHandle), ResourceStates::UnorderedAccess);
				break;

			case ResourceType::ConstantBuffer:
				requireBufferState(CHECKED_CAST<IBuffer*>(binding.resourceHandle), ResourceStates::ConstantBuffer);
				break;

			case ResourceType::RayTracingAccelStruct:
				requireBufferState(CHECKED_CAST<AccelStruct*>(binding.resourceHandle)->dataBuffer, ResourceStates::AccelStructRead);
				break;

			default:
				// do nothing
				break;
			}
		}
	}

	void CommandList::requireTextureState(ITexture* _texture, TextureSubresourceSet subresources, ResourceStates state)
	{
		Texture* texture = CHECKED_CAST<Texture*>(_texture);

		m_StateTracker.requireTextureState(texture, subresources, state);
	}

	void CommandList::requireBufferState(IBuffer* _buffer, ResourceStates state)
	{
		Buffer* buffer = CHECKED_CAST<Buffer*>(_buffer);

		m_StateTracker.requireBufferState(buffer, state);
	}

	void CommandList::commitBarriers()
	{
		const auto& textureBarriers = m_StateTracker.getTextureBarriers();
		const auto& bufferBarriers = m_StateTracker.getBufferBarriers();
		const size_t barrierCount = textureBarriers.size() + bufferBarriers.size();
		if (barrierCount == 0)
			return;

		// Allocate vector space for the barriers assuming 1:1 translation.
		// For partial transitions on multi-plane textures, original barriers may translate
		// into more than 1 barrier each, but that's relatively rare.
		m_D3DBarriers.clear();
		m_D3DBarriers.reserve(barrierCount);

		// Convert the texture barriers into D3D equivalents
		for (const auto& barrier : textureBarriers)
		{
			const Texture* texture = static_cast<const Texture*>(barrier.texture);

			D3D12_RESOURCE_BARRIER d3dbarrier{};
			const D3D12_RESOURCE_STATES stateBefore = convertResourceStates(barrier.stateBefore);
			const D3D12_RESOURCE_STATES stateAfter = convertResourceStates(barrier.stateAfter);
			if (stateBefore != stateAfter)
			{
				d3dbarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
				d3dbarrier.Transition.StateBefore = stateBefore;
				d3dbarrier.Transition.StateAfter = stateAfter;
				d3dbarrier.Transition.pResource = texture->resource;
				if (barrier.entireTexture)
				{
					d3dbarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
					m_D3DBarriers.push_back(d3dbarrier);
				}
				else
				{
					for (uint8_t plane = 0; plane < texture->planeCount; plane++)
					{
						d3dbarrier.Transition.Subresource = calcSubresource(barrier.mipLevel, barrier.arraySlice, plane, texture->desc.mipLevels, texture->desc.arraySize);
						m_D3DBarriers.push_back(d3dbarrier);
					}
				}
			}
			else if (stateAfter & D3D12_RESOURCE_STATE_UNORDERED_ACCESS)
			{
				d3dbarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
				d3dbarrier.UAV.pResource = texture->resource;
				m_D3DBarriers.push_back(d3dbarrier);
			}
		}

		// Convert the buffer barriers into D3D equivalents
		for (const auto& barrier : bufferBarriers)
		{
			const Buffer* buffer = static_cast<const Buffer*>(barrier.buffer);

			D3D12_RESOURCE_BARRIER d3dbarrier{};
			const D3D12_RESOURCE_STATES stateBefore = convertResourceStates(barrier.stateBefore);
			const D3D12_RESOURCE_STATES stateAfter = convertResourceStates(barrier.stateAfter);
			if (stateBefore != stateAfter &&
				(stateBefore & D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE) == 0 &&
				(stateAfter & D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE) == 0)
			{
				d3dbarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
				d3dbarrier.Transition.StateBefore = stateBefore;
				d3dbarrier.Transition.StateAfter = stateAfter;
				d3dbarrier.Transition.pResource = buffer->resource;
				d3dbarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
				m_D3DBarriers.push_back(d3dbarrier);
			}
			else if ((barrier.stateBefore == ResourceStates::AccelStructWrite && (barrier.stateAfter & (ResourceStates::AccelStructRead | ResourceStates::AccelStructBuildBlas)) != 0) ||
				(barrier.stateAfter == ResourceStates::AccelStructWrite && (barrier.stateBefore & (ResourceStates::AccelStructRead | ResourceStates::AccelStructBuildBlas)) != 0) ||
				(stateAfter & D3D12_RESOURCE_STATE_UNORDERED_ACCESS) != 0)
			{
				d3dbarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
				d3dbarrier.UAV.pResource = buffer->resource;
				m_D3DBarriers.push_back(d3dbarrier);
			}
		}

		assert(!m_D3DBarriers.empty()); // otherwise there's an early-out in the beginning of this function

		m_ActiveCommandList->commandList->ResourceBarrier(uint32_t(m_D3DBarriers.size()), m_D3DBarriers.data());

		m_StateTracker.clearBarriers();
	}

	void CommandList::setEnableAutomaticBarriers(bool enable)
	{
		m_EnableAutomaticBarriers = enable;
	}

	void CommandList::setEnableUavBarriersForTexture(ITexture* _texture, bool enableBarriers)
	{
		Texture* texture = CHECKED_CAST<Texture*>(_texture);

		m_StateTracker.setEnableUavBarriersForTexture(texture, enableBarriers);
	}

	void CommandList::setEnableUavBarriersForBuffer(IBuffer* _buffer, bool enableBarriers)
	{
		Buffer* buffer = CHECKED_CAST<Buffer*>(_buffer);

		m_StateTracker.setEnableUavBarriersForBuffer(buffer, enableBarriers);
	}

	ShaderTableState* CommandList::getShaderTableStateTracking(rt::IShaderTable* shaderTable)
	{
		auto it = m_ShaderTableStates.find(shaderTable);

		if (it != m_ShaderTableStates.end())
		{
			return it->second.get();
		}

		std::unique_ptr<ShaderTableState> trackingRef = std::make_unique<ShaderTableState>();

		ShaderTableState* tracking = trackingRef.get();
		m_ShaderTableStates.insert(std::make_pair(shaderTable, std::move(trackingRef)));

		return tracking;
	}

	void CommandList::beginTrackingTextureState(ITexture* _texture, TextureSubresourceSet subresources, ResourceStates stateBits)
	{
		Texture* texture = CHECKED_CAST<Texture*>(_texture);

		m_StateTracker.beginTrackingTextureState(texture, subresources, stateBits);
	}

	void CommandList::beginTrackingBufferState(IBuffer* _buffer, ResourceStates stateBits)
	{
		Buffer* buffer = CHECKED_CAST<Buffer*>(_buffer);

		m_StateTracker.beginTrackingBufferState(buffer, stateBits);
	}

	void CommandList::setTextureState(ITexture* _texture, TextureSubresourceSet subresources, ResourceStates stateBits)
	{
		Texture* texture = CHECKED_CAST<Texture*>(_texture);

		m_StateTracker.endTrackingTextureState(texture, subresources, stateBits, false);
	}

	void CommandList::setBufferState(IBuffer* _buffer, ResourceStates stateBits)
	{
		Buffer* buffer = CHECKED_CAST<Buffer*>(_buffer);

		m_StateTracker.endTrackingBufferState(buffer, stateBits, false);
	}

	void CommandList::setAccelStructState(rt::IAccelStruct* _as, ResourceStates stateBits)
	{
		AccelStruct* as = CHECKED_CAST<AccelStruct*>(_as);

		if (as->dataBuffer)
			m_StateTracker.endTrackingBufferState(as->dataBuffer, stateBits, false);
	}

	void CommandList::setPermanentTextureState(ITexture* _texture, ResourceStates stateBits)
	{
		Texture* texture = CHECKED_CAST<Texture*>(_texture);

		m_StateTracker.endTrackingTextureState(texture, AllSubresources, stateBits, true);

	}

	void CommandList::setPermanentBufferState(IBuffer* _buffer, ResourceStates stateBits)
	{
		Buffer* buffer = CHECKED_CAST<Buffer*>(_buffer);

		m_StateTracker.endTrackingBufferState(buffer, stateBits, true);
	}

	ResourceStates CommandList::getTextureSubresourceState(ITexture* _texture, ArraySlice arraySlice, MipLevel mipLevel)
	{
		Texture* texture = CHECKED_CAST<Texture*>(_texture);

		return m_StateTracker.getTextureSubresourceState(texture, arraySlice, mipLevel);
	}

	ResourceStates CommandList::getBufferState(IBuffer* _buffer)
	{
		Buffer* buffer = CHECKED_CAST<Buffer*>(_buffer);

		return m_StateTracker.getBufferState(buffer);
	}

}
}