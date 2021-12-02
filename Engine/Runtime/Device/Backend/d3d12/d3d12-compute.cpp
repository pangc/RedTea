#include "d3d12-backend.h"


#include "../../RHI/misc.h"
#include <sstream>

namespace redtea
{
namespace device
{
    Object ComputePipeline::getNativeObject(ObjectType objectType)
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


    RefCountPtr<ID3D12PipelineState> Device::createPipelineState(const ComputePipelineDesc& state, RootSignature* pRS) const
    {
        RefCountPtr<ID3D12PipelineState> pipelineState;

        D3D12_COMPUTE_PIPELINE_STATE_DESC desc = {};

        desc.pRootSignature = pRS->handle;
        Shader* shader = CHECKED_CAST<Shader*>(state.CS.Get());
        desc.CS = { &shader->bytecode[0], shader->bytecode.size() };

        const HRESULT hr = m_Context.device->CreateComputePipelineState(&desc, IID_PPV_ARGS(&pipelineState));

        if (FAILED(hr))
        {
            m_Context.error("Failed to create a compute pipeline state object");
            return nullptr;
        }

        return pipelineState;
    }

    ComputePipelineHandle Device::createComputePipeline(const ComputePipelineDesc& desc)
    {
        RefCountPtr<RootSignature> pRS = getRootSignature(desc.bindingLayouts, false);
        RefCountPtr<ID3D12PipelineState> pPSO = createPipelineState(desc, pRS);

        if (pPSO == nullptr)
            return nullptr;

        ComputePipeline* pso = new ComputePipeline();

        pso->desc = desc;

        pso->rootSignature = pRS;
        pso->pipelineState = pPSO;

        return ComputePipelineHandle::Create(pso);
    }

    void CommandList::setComputeState(const ComputeState& state)
    {
        ComputePipeline* pso = CHECKED_CAST<ComputePipeline*>(state.pipeline);

        const bool updateRootSignature = !m_CurrentComputeStateValid || m_CurrentComputeState.pipeline == nullptr ||
            CHECKED_CAST<ComputePipeline*>(m_CurrentComputeState.pipeline)->rootSignature != pso->rootSignature;

        bool updatePipeline = !m_CurrentComputeStateValid || m_CurrentComputeState.pipeline != state.pipeline;
        bool updateIndirectParams = !m_CurrentComputeStateValid || m_CurrentComputeState.indirectParams != state.indirectParams;

        uint32_t bindingUpdateMask = 0;
        if (!m_CurrentComputeStateValid || updateRootSignature)
            bindingUpdateMask = ~0u;

        if (commitDescriptorHeaps())
            bindingUpdateMask = ~0u;

        if (bindingUpdateMask == 0)
            bindingUpdateMask = ArrayDifferenceMask(m_CurrentComputeState.bindings, state.bindings);

        if (updateRootSignature)
        {
            m_ActiveCommandList->commandList->SetComputeRootSignature(pso->rootSignature->handle);
        }

        if (updatePipeline)
        {
            m_ActiveCommandList->commandList->SetPipelineState(pso->pipelineState);

            m_Instance->referencedResources.push_back(pso);
        }

        setComputeBindings(state.bindings, bindingUpdateMask, state.indirectParams, updateIndirectParams, pso->rootSignature);

        unbindShadingRateState();

        m_CurrentGraphicsStateValid = false;
        m_CurrentComputeStateValid = true;
        m_CurrentMeshletStateValid = false;
        m_CurrentRayTracingStateValid = false;
        m_CurrentComputeState = state;

        commitBarriers();
    }

    void CommandList::updateComputeVolatileBuffers()
    {
        // If there are some volatile buffers bound, and they have been written into since the last dispatch or setComputeState, patch their views
        if (!m_AnyVolatileBufferWrites)
            return;

        for (VolatileConstantBufferBinding& parameter : m_CurrentComputeVolatileCBs)
        {
            const D3D12_GPU_VIRTUAL_ADDRESS currentGpuVA = m_VolatileConstantBufferAddresses[parameter.buffer];

            if (currentGpuVA != parameter.address)
            {
                m_ActiveCommandList->commandList->SetComputeRootConstantBufferView(parameter.bindingPoint, currentGpuVA);

                parameter.address = currentGpuVA;
            }
        }

        m_AnyVolatileBufferWrites = false;
    }

    void CommandList::dispatch(uint32_t groupsX, uint32_t groupsY, uint32_t groupsZ)
    {
        updateComputeVolatileBuffers();

        m_ActiveCommandList->commandList->Dispatch(groupsX, groupsY, groupsZ);
    }

    void CommandList::dispatchIndirect(uint32_t offsetBytes)
    {
        Buffer* indirectParams = CHECKED_CAST<Buffer*>(m_CurrentComputeState.indirectParams);
        assert(indirectParams); // validation layer handles this

        updateComputeVolatileBuffers();

        m_ActiveCommandList->commandList->ExecuteIndirect(m_Context.dispatchIndirectSignature, 1, indirectParams->resource, offsetBytes, nullptr, 0);
    }
}
}
