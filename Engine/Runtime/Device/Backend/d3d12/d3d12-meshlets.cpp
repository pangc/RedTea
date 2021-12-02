#include "d3d12-backend.h"

#include "../../RHI/misc.h"
#include <sstream>

namespace redtea
{
namespace device
{
	Object MeshletPipeline::getNativeObject(ObjectType objectType)
	{
		switch (objectType)
		{
		case ObjectTypes::D3D12_RootSignature:
			return rootSignature->getNativeObject(objectType);
		case ObjectTypes::D3D12_PipelineState:
			return Object(pipelineState.Get());
		default:
			return nullptr;
		}
	}

	RefCountPtr<ID3D12PipelineState> Device::createPipelineState(const MeshletPipelineDesc& state, RootSignature* pRS, const FramebufferInfo& fbinfo) const
	{
		RefCountPtr<ID3D12PipelineState> pipelineState;

#pragma warning(push)
#pragma warning(disable: 4324) // structure was padded due to alignment specifier
		struct PSO_STREAM
		{
			typedef __declspec(align(sizeof(void*))) D3D12_PIPELINE_STATE_SUBOBJECT_TYPE ALIGNED_TYPE;

			ALIGNED_TYPE RootSignature_Type;        ID3D12RootSignature* RootSignature;
			ALIGNED_TYPE PrimitiveTopology_Type;    D3D12_PRIMITIVE_TOPOLOGY_TYPE PrimitiveTopologyType;
			ALIGNED_TYPE AmplificationShader_Type;  D3D12_SHADER_BYTECODE AmplificationShader;
			ALIGNED_TYPE MeshShader_Type;           D3D12_SHADER_BYTECODE MeshShader;
			ALIGNED_TYPE PixelShader_Type;          D3D12_SHADER_BYTECODE PixelShader;
			ALIGNED_TYPE RasterizerState_Type;      D3D12_RASTERIZER_DESC RasterizerState;
			ALIGNED_TYPE DepthStencilState_Type;    D3D12_DEPTH_STENCIL_DESC DepthStencilState;
			ALIGNED_TYPE BlendState_Type;           D3D12_BLEND_DESC BlendState;
			ALIGNED_TYPE SampleDesc_Type;           DXGI_SAMPLE_DESC SampleDesc;
			ALIGNED_TYPE SampleMask_Type;           UINT SampleMask;
			ALIGNED_TYPE RenderTargets_Type;        D3D12_RT_FORMAT_ARRAY RenderTargets;
			ALIGNED_TYPE DSVFormat_Type;            DXGI_FORMAT DSVFormat;
		} psoDesc = { };
#pragma warning(pop)

		psoDesc.RootSignature_Type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_ROOT_SIGNATURE;
		psoDesc.PrimitiveTopology_Type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_PRIMITIVE_TOPOLOGY;
		psoDesc.AmplificationShader_Type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_AS;
		psoDesc.MeshShader_Type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_MS;
		psoDesc.PixelShader_Type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_PS;
		psoDesc.RasterizerState_Type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_RASTERIZER;
		psoDesc.DepthStencilState_Type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL;
		psoDesc.BlendState_Type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_BLEND;
		psoDesc.SampleDesc_Type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_SAMPLE_DESC;
		psoDesc.SampleMask_Type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_SAMPLE_MASK;
		psoDesc.RenderTargets_Type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_RENDER_TARGET_FORMATS;
		psoDesc.DSVFormat_Type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL_FORMAT;

		psoDesc.RootSignature = pRS->handle;

		TranslateBlendState(state.renderState.blendState, psoDesc.BlendState);

		const DepthStencilState& depthState = state.renderState.depthStencilState;
		TranslateDepthStencilState(depthState, psoDesc.DepthStencilState);

		if ((depthState.depthTestEnable || depthState.stencilEnable) && fbinfo.depthFormat == Format::UNKNOWN)
		{
			psoDesc.DepthStencilState.DepthEnable = FALSE;
			psoDesc.DepthStencilState.StencilEnable = FALSE;
		}

		const RasterState& rasterState = state.renderState.rasterState;
		TranslateRasterizerState(rasterState, psoDesc.RasterizerState);

		switch (state.primType)
		{
		case PrimitiveType::PointList:
			psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
			break;
		case PrimitiveType::LineList:
			psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
			break;
		case PrimitiveType::TriangleList:
		case PrimitiveType::TriangleStrip:
			psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			break;
		case PrimitiveType::PatchList:
			m_Context.error("Unsupported primitive topology for meshlets");
			return nullptr;
		default:
			utils::InvalidEnum();
			return nullptr;
		}

		psoDesc.SampleDesc.Count = fbinfo.sampleCount;
		psoDesc.SampleDesc.Quality = fbinfo.sampleQuality;
		psoDesc.SampleMask = ~0u;

		for (uint32_t i = 0; i < uint32_t(fbinfo.colorFormats.size()); i++)
		{
			psoDesc.RenderTargets.RTFormats[i] = getDxgiFormatMapping(fbinfo.colorFormats[i]).rtvFormat;
		}
		psoDesc.RenderTargets.NumRenderTargets = uint32_t(fbinfo.colorFormats.size());

		psoDesc.DSVFormat = getDxgiFormatMapping(fbinfo.depthFormat).rtvFormat;

		if (state.AS)
		{
			state.AS->getBytecode(&psoDesc.AmplificationShader.pShaderBytecode, &psoDesc.AmplificationShader.BytecodeLength);
		}

		if (state.MS)
		{
			state.MS->getBytecode(&psoDesc.MeshShader.pShaderBytecode, &psoDesc.MeshShader.BytecodeLength);
		}

		if (state.PS)
		{
			state.PS->getBytecode(&psoDesc.PixelShader.pShaderBytecode, &psoDesc.PixelShader.BytecodeLength);
		}

		D3D12_PIPELINE_STATE_STREAM_DESC streamDesc;
		streamDesc.pPipelineStateSubobjectStream = &psoDesc;
		streamDesc.SizeInBytes = sizeof(psoDesc);

		HRESULT hr = m_Context.device2->CreatePipelineState(&streamDesc, IID_PPV_ARGS(&pipelineState));
		if (FAILED(hr))
		{
			m_Context.error("Failed to create a meshlet pipeline state object");
			return nullptr;
		}

		return pipelineState;
	}

	MeshletPipelineHandle Device::createMeshletPipeline(const MeshletPipelineDesc& desc, IFramebuffer* fb)
	{
		RefCountPtr<RootSignature> pRS = getRootSignature(desc.bindingLayouts, false);

		RefCountPtr<ID3D12PipelineState> pPSO = createPipelineState(desc, pRS, fb->getFramebufferInfo());

		return createHandleForNativeMeshletPipeline(pRS, pPSO, desc, fb->getFramebufferInfo());
	}

	MeshletPipelineHandle Device::createHandleForNativeMeshletPipeline(IRootSignature* rootSignature, ID3D12PipelineState* pipelineState, const MeshletPipelineDesc& desc, const FramebufferInfo& framebufferInfo)
	{
		if (rootSignature == nullptr)
			return nullptr;

		if (pipelineState == nullptr)
			return nullptr;

		MeshletPipeline *pso = new MeshletPipeline();
		pso->desc = desc;
		pso->framebufferInfo = framebufferInfo;
		pso->rootSignature = CHECKED_CAST<RootSignature*>(rootSignature);
		pso->pipelineState = pipelineState;
		pso->requiresBlendFactor = desc.renderState.blendState.usesConstantColor(uint32_t(pso->framebufferInfo.colorFormats.size()));

		return MeshletPipelineHandle::Create(pso);
	}

	void CommandList::bindMeshletPipeline(MeshletPipeline *pso, bool updateRootSignature) const
	{
		const auto& state = pso->desc;

		ID3D12GraphicsCommandList* commandList = m_ActiveCommandList->commandList;

		if (updateRootSignature)
		{
			commandList->SetGraphicsRootSignature(pso->rootSignature->handle);
		}

		commandList->SetPipelineState(pso->pipelineState);

		commandList->IASetPrimitiveTopology(convertPrimitiveType(state.primType, 0));

		if (pso->viewportState.numViewports)
		{
			commandList->RSSetViewports(pso->viewportState.numViewports, pso->viewportState.viewports);
		}

		if (pso->viewportState.numScissorRects)
		{
			commandList->RSSetScissorRects(pso->viewportState.numViewports, pso->viewportState.scissorRects);
		}

		if (state.renderState.depthStencilState.stencilEnable)
		{
			commandList->OMSetStencilRef(state.renderState.depthStencilState.stencilRefValue);
		}
	}

	void CommandList::setMeshletState(const MeshletState& state)
	{
		MeshletPipeline* pso = CHECKED_CAST<MeshletPipeline*>(state.pipeline);
		Framebuffer* framebuffer = CHECKED_CAST<Framebuffer*>(state.framebuffer);

		unbindShadingRateState();

		const bool updateFramebuffer = !m_CurrentMeshletStateValid || m_CurrentMeshletState.framebuffer != state.framebuffer;
		const bool updateRootSignature = !m_CurrentMeshletStateValid || m_CurrentMeshletState.pipeline == nullptr ||
			CHECKED_CAST<MeshletPipeline*>(m_CurrentMeshletState.pipeline)->rootSignature != pso->rootSignature;

		const bool updatePipeline = !m_CurrentMeshletStateValid || m_CurrentMeshletState.pipeline != state.pipeline;
		const bool updateIndirectParams = !m_CurrentMeshletStateValid || m_CurrentMeshletState.indirectParams != state.indirectParams;

		const bool updateViewports = !m_CurrentMeshletStateValid ||
			ArraysAreDifferent(m_CurrentMeshletState.viewport.viewports, state.viewport.viewports) ||
			ArraysAreDifferent(m_CurrentMeshletState.viewport.scissorRects, state.viewport.scissorRects);

		const bool updateBlendFactor = !m_CurrentGraphicsStateValid || m_CurrentGraphicsState.blendConstantColor != state.blendConstantColor;

		uint32_t bindingUpdateMask = 0;
		if (!m_CurrentMeshletStateValid || updateRootSignature)
			bindingUpdateMask = ~0u;

		if (commitDescriptorHeaps())
			bindingUpdateMask = ~0u;

		if (bindingUpdateMask == 0)
			bindingUpdateMask = ArrayDifferenceMask(m_CurrentMeshletState.bindings, state.bindings);

		if (updatePipeline)
		{
			bindMeshletPipeline(pso, updateRootSignature);
			m_Instance->referencedResources.push_back(pso);
		}

		if (pso->requiresBlendFactor && updateBlendFactor)
		{
			m_ActiveCommandList->commandList->OMSetBlendFactor(&state.blendConstantColor.r);
		}

		if (updateFramebuffer)
		{
			bindFramebuffer(pso->desc.renderState.depthStencilState, framebuffer);
			m_Instance->referencedResources.push_back(framebuffer);
		}

		setGraphicsBindings(state.bindings, bindingUpdateMask, state.indirectParams, updateIndirectParams, pso->rootSignature);

		commitBarriers();

		if (updateViewports)
		{
			DX12_ViewportState vpState = convertViewportState(pso->desc.renderState.rasterState, framebuffer->framebufferInfo, state.viewport);

			if (vpState.numViewports)
			{
				assert(pso->viewportState.numViewports == 0);
				m_ActiveCommandList->commandList->RSSetViewports(vpState.numViewports, vpState.viewports);
			}

			if (vpState.numScissorRects)
			{
				assert(pso->viewportState.numScissorRects == 0);
				m_ActiveCommandList->commandList->RSSetScissorRects(vpState.numScissorRects, vpState.scissorRects);
			}
		}

		m_CurrentGraphicsStateValid = false;
		m_CurrentComputeStateValid = false;
		m_CurrentMeshletStateValid = true;
		m_CurrentRayTracingStateValid = false;
		m_CurrentMeshletState = state;
	}

	void CommandList::dispatchMesh(uint32_t groupsX, uint32_t groupsY /*= 1*/, uint32_t groupsZ /*= 1*/)
	{
		updateGraphicsVolatileBuffers();

		m_ActiveCommandList->commandList6->DispatchMesh(groupsX, groupsY, groupsZ);
	}
}
}