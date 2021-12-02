
#include "d3d12-backend.h"
#include <pix.h>
#include <sstream>

#include "../../RHI/misc.h"

namespace redtea
{
namespace device
{
	CommandList::CommandList(Device* device, const Context& context, DeviceResources& resources, const CommandListParameters& params)
		: m_Context(context)
		, m_Resources(resources)
		, m_Device(device)
		, m_Queue(device->getQueue(params.queueType))
		, m_UploadManager(context, m_Queue, params.uploadChunkSize, 0, false)
		, m_DxrScratchManager(context, m_Queue, params.scratchChunkSize, params.scratchMaxMemory, true)
		, m_StateTracker(context.messageCallback)
		, m_Desc(params)
	{
	}

	Object CommandList::getNativeObject(ObjectType objectType)
	{
		switch (objectType)
		{
		case ObjectTypes::D3D12_GraphicsCommandList:
			if (m_ActiveCommandList)
				return Object(m_ActiveCommandList->commandList.Get());
			else
				return nullptr;

		case ObjectTypes::Nvrhi_D3D12_CommandList:
			return this;

		default:
			return nullptr;
		}
	}

	std::shared_ptr<InternalCommandList> CommandList::createInternalCommandList() const
	{
		auto commandList = std::make_shared<InternalCommandList>();

		D3D12_COMMAND_LIST_TYPE d3dCommandListType;
		switch (m_Desc.queueType)
		{
		case CommandQueue::Graphics:
			d3dCommandListType = D3D12_COMMAND_LIST_TYPE_DIRECT;
			break;
		case CommandQueue::Compute:
			d3dCommandListType = D3D12_COMMAND_LIST_TYPE_COMPUTE;
			break;
		case CommandQueue::Copy:
			d3dCommandListType = D3D12_COMMAND_LIST_TYPE_COPY;
			break;

		case CommandQueue::Count:
		default:
			utils::InvalidEnum();
			return nullptr;
		}

		m_Context.device->CreateCommandAllocator(d3dCommandListType, IID_PPV_ARGS(&commandList->allocator));
		m_Context.device->CreateCommandList(0, d3dCommandListType, commandList->allocator, nullptr, IID_PPV_ARGS(&commandList->commandList));

		commandList->commandList->QueryInterface(IID_PPV_ARGS(&commandList->commandList4));
		commandList->commandList->QueryInterface(IID_PPV_ARGS(&commandList->commandList6));

		return commandList;
	}

	bool CommandList::commitDescriptorHeaps()
	{
		ID3D12DescriptorHeap* heapSRVetc = m_Resources.shaderResourceViewHeap.getShaderVisibleHeap();
		ID3D12DescriptorHeap* heapSamplers = m_Resources.samplerHeap.getShaderVisibleHeap();

		if (heapSRVetc != m_CurrentHeapSRVetc || heapSamplers != m_CurrentHeapSamplers)
		{
			ID3D12DescriptorHeap* heaps[2] = { heapSRVetc, heapSamplers };
			m_ActiveCommandList->commandList->SetDescriptorHeaps(2, heaps);

			m_CurrentHeapSRVetc = heapSRVetc;
			m_CurrentHeapSamplers = heapSamplers;

			m_Instance->referencedNativeResources.push_back(heapSRVetc);
			m_Instance->referencedNativeResources.push_back(heapSamplers);

			return true;
		}

		return false;
	}

	bool CommandList::allocateUploadBuffer(size_t size, void** pCpuAddress, D3D12_GPU_VIRTUAL_ADDRESS* pGpuAddress)
	{
		return m_UploadManager.suballocateBuffer(size, nullptr, nullptr, nullptr, pCpuAddress, pGpuAddress,
			m_RecordingVersion, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
	}

	D3D12_GPU_VIRTUAL_ADDRESS CommandList::getBufferGpuVA(IBuffer* _buffer)
	{
		if (!_buffer)
			return 0;

		Buffer* buffer = CHECKED_CAST<Buffer*>(_buffer);

		if (buffer->desc.isVolatile)
		{
			return m_VolatileConstantBufferAddresses[buffer];
		}

		return buffer->gpuVA;
	}

	IDevice* CommandList::getDevice()
	{
		return m_Device;
	}

	void CommandList::beginMarker(const char* name)
	{
		PIXBeginEvent(m_ActiveCommandList->commandList, 0, name);
	}

	void CommandList::endMarker()
	{
		PIXEndEvent(m_ActiveCommandList->commandList);
	}

	void CommandList::setPushConstants(const void* data, size_t byteSize)
	{
		const RootSignature* rootsig = nullptr;
		bool isGraphics = false;

		if (m_CurrentGraphicsStateValid && m_CurrentGraphicsState.pipeline)
		{
			GraphicsPipeline* pso = CHECKED_CAST<GraphicsPipeline*>(m_CurrentGraphicsState.pipeline);
			rootsig = pso->rootSignature;
			isGraphics = true;
		}
		else if (m_CurrentComputeStateValid && m_CurrentComputeState.pipeline)
		{
			ComputePipeline* pso = CHECKED_CAST<ComputePipeline*>(m_CurrentComputeState.pipeline);
			rootsig = pso->rootSignature;
			isGraphics = false;
		}
		else if (m_CurrentRayTracingStateValid && m_CurrentRayTracingState.shaderTable)
		{
			RayTracingPipeline* pso = CHECKED_CAST<RayTracingPipeline*>(m_CurrentRayTracingState.shaderTable->getPipeline());
			rootsig = pso->globalRootSignature;
			isGraphics = false;
		}
		else if (m_CurrentMeshletStateValid && m_CurrentMeshletState.pipeline)
		{
			MeshletPipeline* pso = CHECKED_CAST<MeshletPipeline*>(m_CurrentMeshletState.pipeline);
			rootsig = pso->rootSignature;
			isGraphics = true;
		}

		if (!rootsig || !rootsig->pushConstantByteSize)
			return;

		assert(byteSize == rootsig->pushConstantByteSize); // the validation error handles the error message

		if (isGraphics)
			m_ActiveCommandList->commandList->SetGraphicsRoot32BitConstants(rootsig->rootParameterPushConstants, UINT(byteSize / 4), data, 0);
		else
			m_ActiveCommandList->commandList->SetComputeRoot32BitConstants(rootsig->rootParameterPushConstants, UINT(byteSize / 4), data, 0);
	}

	void CommandList::open()
	{
		uint64_t completedInstance = m_Queue->updateLastCompletedInstance();

		std::shared_ptr<InternalCommandList> chunk;

		if (!m_CommandListPool.empty())
		{
			chunk = m_CommandListPool.front();

			if (chunk->lastSubmittedInstance <= completedInstance)
			{
				chunk->allocator->Reset();
				chunk->commandList->Reset(chunk->allocator, nullptr);
				m_CommandListPool.pop_front();
			}
			else
			{
				chunk = nullptr;
			}
		}

		if (chunk == nullptr)
		{
			chunk = createInternalCommandList();
		}

		m_ActiveCommandList = chunk;

		m_Instance = std::make_shared<CommandListInstance>();
		m_Instance->commandAllocator = m_ActiveCommandList->allocator;
		m_Instance->commandList = m_ActiveCommandList->commandList;
		m_Instance->commandQueue = m_Desc.queueType;

		m_RecordingVersion = MakeVersion(m_Queue->recordingInstance++, m_Desc.queueType, false);
	}

	void CommandList::clearStateCache()
	{
		m_AnyVolatileBufferWrites = false;
		m_CurrentGraphicsStateValid = false;
		m_CurrentComputeStateValid = false;
		m_CurrentMeshletStateValid = false;
		m_CurrentRayTracingStateValid = false;
		m_CurrentHeapSRVetc = nullptr;
		m_CurrentHeapSamplers = nullptr;
		m_CurrentGraphicsVolatileCBs.resize(0);
		m_CurrentComputeVolatileCBs.resize(0);
		m_CurrentSinglePassStereoState = SinglePassStereoState();
	}

	void CommandList::clearState()
	{
		m_ActiveCommandList->commandList->ClearState(nullptr);

		clearStateCache();

		commitDescriptorHeaps();
	}

	void CommandList::close()
	{
		m_StateTracker.keepBufferInitialStates();
		m_StateTracker.keepTextureInitialStates();
		commitBarriers();

		m_ActiveCommandList->commandList->Close();

		clearStateCache();

		m_CurrentUploadBuffer = nullptr;
		m_VolatileConstantBufferAddresses.clear();
		m_ShaderTableStates.clear();
	}

	std::shared_ptr<CommandListInstance> CommandList::executed(Queue* pQueue)
	{
		std::shared_ptr<CommandListInstance> instance = m_Instance;
		instance->fence = pQueue->fence;
		instance->submittedInstance = pQueue->lastSubmittedInstance;
		m_Instance.reset();

		m_ActiveCommandList->lastSubmittedInstance = pQueue->lastSubmittedInstance;
		m_CommandListPool.push_back(m_ActiveCommandList);
		m_ActiveCommandList.reset();

		for (const auto& it : instance->referencedStagingTextures)
		{
			it->lastUseFence = pQueue->fence;
			it->lastUseFenceValue = instance->submittedInstance;
		}

		for (const auto& it : instance->referencedStagingBuffers)
		{
			it->lastUseFence = pQueue->fence;
			it->lastUseFenceValue = instance->submittedInstance;
		}

		for (const auto& it : instance->referencedTimerQueries)
		{
			it->started = true;
			it->resolved = false;
			it->fence = pQueue->fence;
			it->fenceCounter = instance->submittedInstance;
		}

		m_StateTracker.commandListSubmitted();

		uint64_t submittedVersion = MakeVersion(instance->submittedInstance, m_Desc.queueType, true);
		m_UploadManager.submitChunks(m_RecordingVersion, submittedVersion);
		m_DxrScratchManager.submitChunks(m_RecordingVersion, submittedVersion);
		m_RecordingVersion = 0;

		return instance;
	}

}
}