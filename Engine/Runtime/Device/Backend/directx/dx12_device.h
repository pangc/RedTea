#pragma once
#include "../../RHI/rhi.h"
#include "../../RHI/device.h"
#include <d3d12.h>
#include <dxgi1_6.h>

namespace redtea
{
namespace device
{
	class DX12Device : public IDevice
	{
	public:
		virtual ~DX12Device() override;
		virtual bool InitDevice(void* window) override;
		virtual GraphicsAPI GetGraphicsAPI() override { return GraphicsAPI::DX12; }
		virtual SwapChainHandle CreateSwapchain(const SwapChainDesc& d) override;
		virtual void MakeCurrent(SwapChainHandle draw, SwapChainHandle read) override;
		virtual HeapHandle CreateHeap(const HeapDesc& d) override;
		virtual TextureHandle CreateTexture(const TextureDesc& d) override;
		virtual SamplerStateHandle CreateSampler(const SamplerDesc& d) override;
		virtual IndexBufferHandle CreateIndexBuffer(const BufferDesc &d) override;
		virtual ShaderHandle CreateShader(const ShaderDesc& d) override;
		virtual RenderPrimitiveHandle CreateRenderPrimitive() override;
		virtual FrameBufferHandle CreateFrameBuffer() override;
		virtual PipelineStateHandle CreatePiplelineState(const PipelineDesc& d) override;
		virtual ComputePipelineState CreateComputePipelineState(const ComputePipelineDesc& d) override;
	private:
		void FindAdaptor(IDXGIFactory1* pFactory, IDXGIAdapter1** ppAdapter);
        ID3D12Device* mDevice = nullptr;
		IDXGIFactory4* mFactory = nullptr;
		ID3D12CommandQueue* mQueue = nullptr;
		void* nativeWindow;
	};
}
}