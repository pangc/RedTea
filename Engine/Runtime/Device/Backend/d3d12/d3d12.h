#pragma once

#include "../../RHI/rhi.h"
#include <common.h>

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <d3d12.h>

#define LOGIfFailed(hr, msg) if(FAILED(hr)){LOGE(msg); return false;}

namespace redtea {
namespace device {
    namespace ObjectTypes
    {
        constexpr ObjectType D3D12_DXDevice         = 0x00020101;
        constexpr ObjectType D3D12_DXCommandList    = 0x00020102;
    };
}
}

namespace redtea {
namespace device {

    class IRootSignature : public IResource
    {
    };

    typedef RefCountPtr<IRootSignature> RootSignatureHandle;

    class DXCommandList : public ICommandList
    {
    public:
        virtual bool allocateUploadBuffer(size_t size, void** pCpuAddress, D3D12_GPU_VIRTUAL_ADDRESS* pGpuAddress) = 0;
        virtual bool commitDescriptorHeaps() = 0;
        virtual D3D12_GPU_VIRTUAL_ADDRESS getBufferGpuVA(IBuffer* buffer) = 0;

        virtual void updateGraphicsVolatileBuffers() = 0;
        virtual void updateComputeVolatileBuffers() = 0;
    };

    typedef RefCountPtr<DXCommandList> DXCommandListHandle;

    typedef uint32_t DescriptorIndex;

    class IDescriptorHeap
    {
    protected:
        IDescriptorHeap() = default;
        virtual ~IDescriptorHeap() = default;
    public:
        virtual DescriptorIndex allocateDescriptors(uint32_t count) = 0;
        virtual DescriptorIndex allocateDescriptor() = 0;
        virtual void releaseDescriptors(DescriptorIndex baseIndex, uint32_t count) = 0;
        virtual void releaseDescriptor(DescriptorIndex index) = 0;
        virtual D3D12_CPU_DESCRIPTOR_HANDLE getCpuHandle(DescriptorIndex index) = 0;
        virtual D3D12_CPU_DESCRIPTOR_HANDLE getCpuHandleShaderVisible(DescriptorIndex index) = 0;
        virtual D3D12_GPU_DESCRIPTOR_HANDLE getGpuHandle(DescriptorIndex index) = 0;
        virtual ID3D12DescriptorHeap* getHeap() const = 0;
        virtual ID3D12DescriptorHeap* getShaderVisibleHeap() const = 0;

        IDescriptorHeap(const IDescriptorHeap&) = delete;
        IDescriptorHeap(const IDescriptorHeap&&) = delete;
        IDescriptorHeap& operator=(const IDescriptorHeap&) = delete;
        IDescriptorHeap& operator=(const IDescriptorHeap&&) = delete;
    };

    enum class DescriptorHeapType
    {
        RenderTargetView,
        DepthStencilView,
        ShaderResrouceView,
        Sampler
    };

    class DXDevice : public IDevice
    {
    public:
        // D3D12-specific methods
        virtual RootSignatureHandle buildRootSignature(const static_vector<BindingLayoutHandle, c_MaxBindingLayouts>& pipelineLayouts, bool allowInputLayout, bool isLocal, const D3D12_ROOT_PARAMETER1* pCustomParameters = nullptr, uint32_t numCustomParameters = 0) = 0;
        virtual GraphicsPipelineHandle createHandleForNativeGraphicsPipeline(IRootSignature* rootSignature, ID3D12PipelineState* pipelineState, const GraphicsPipelineDesc& desc, const FramebufferInfo& framebufferInfo) = 0;
        virtual MeshletPipelineHandle createHandleForNativeMeshletPipeline(IRootSignature* rootSignature, ID3D12PipelineState* pipelineState, const MeshletPipelineDesc& desc, const FramebufferInfo& framebufferInfo) = 0;
        virtual IDescriptorHeap* getDescriptorHeap(DescriptorHeapType heapType) = 0;
        virtual ID3D12Device* getRawDevice() = 0;
    };

    typedef RefCountPtr<DXDevice> DXDeviceHandle;

    struct DeviceDesc
    {
        IMessageCallback* errorCB = nullptr;
        ID3D12Device* pDevice = nullptr;
        ID3D12CommandQueue* pGraphicsCommandQueue = nullptr;
        ID3D12CommandQueue* pComputeCommandQueue = nullptr;
        ID3D12CommandQueue* pCopyCommandQueue = nullptr;
		HWND nativeWindow = nullptr;

        uint32_t renderTargetViewHeapSize = 1024;
        uint32_t depthStencilViewHeapSize = 1024;
        uint32_t shaderResourceViewHeapSize = 16384;
        uint32_t samplerHeapSize = 1024;
        uint32_t timerQueryHeapSize = 256;
    };

    DeviceHandle createDevice(const DeviceDesc& desc);

    DXGI_FORMAT convertFormat(Format format);
}
}