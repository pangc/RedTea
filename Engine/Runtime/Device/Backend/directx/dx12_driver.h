#pragma once
#include "../../RHI/rhi.h"
#include "../../RHI/device.h"
#include <d3d12.h>
#include <dxgi1_6.h>

namespace redtea
{
namespace device
{
    class IDriver
    {
        virtual ~IDriver() {}
		virtual bool InitContext(void* windows) = 0;
		virtual GraphicsAPI GetGraphicsAPI() = 0;
		virtual void CreateSwapchain(SwapChainHandle &swapchain, const SwapChainDesc& d) = 0;
		virtual void MakeCurrent(SwapChainHandle draw, SwapChainHandle read) = 0;
		virtual void CreateHeap(HeapHandle &heap, const HeapDesc& d) = 0;
        virtual void CreateBuffer(BufferHandle &handle, const BufferDesc& desc) = 0;
		virtual void CreateTexture(TextureHandle &texture, const TextureDesc& d) = 0;
		virtual void CreateSampler(SamplerStateHandle &sample, const SamplerDesc& d) = 0;
		virtual void CreateIndexBuffer(IndexBufferHandle &indexbuffer, const BufferDesc &d) = 0;
		virtual void CreateShader(ShaderHandle &shader, const ShaderDesc& d) = 0;
		virtual void CreateRenderPrimitive(RenderPrimitiveHandle &primitive) = 0;
		virtual FrameBufferHandle CreateFrameBuffer() = 0;
		virtual PipelineStateHandle CreatePiplelineState(const PipelineDesc& d) = 0;
		virtual ComputePipelineState CreateComputePipelineState(const ComputePipelineDesc& d) = 0;
    };

    class DX12Context
    {
    public:
        RefCountPtr<ID3D12Device> device = nullptr;
		RefCountPtr<IDXGIFactory4> factory = nullptr;
		RefCountPtr<ID3D12CommandQueue> commandQueue = nullptr;
		void* nativeWindow;
    };

    class DX12Driver : public IDriver
    {
    public:
        virtual bool InitContext(void* windows) override;
        virtual GraphicsAPI GetGraphicsAPI() override { return GraphicsAPI::DX12; }
        virtual void CreateSwapchain(SwapChainHandle &swapchain, const SwapChainDesc& desc) override;
        virtual void CreateBuffer(BufferHandle &handle, const BufferDesc& desc) override;
    private:
        void FindAdaptor(IDXGIFactory1* pFactory, IDXGIAdapter1** ppAdapter);

        DX12Context mContext;
    };
}
}