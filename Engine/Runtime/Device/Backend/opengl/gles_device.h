#pragma once
#include "../../RHI/device.h"
#include "EGL/egl.h"

namespace redtea
{
namespace device
{
	class ESDevice : public IDevice
	{
	public:
		virtual void InitDevice(void* windows) override;
		virtual SwapChainHandle CreateSwapchain() override;
		virtual HeapHandle CreateHeap(const HeapDesc& d) override;
		virtual TextureHandle CreateTexture(const TextureDesc& d) override;
		virtual SamplerStateHandle CreateSampler(const SamplerDesc& d) override;
		virtual VertexBufferHandle CreateVertexBuffer(const BufferDesc &d) override;
		virtual IndexBufferHandle CreateIndexBuffer(const BufferDesc &d) override;
		virtual InputLayoutHandle CreateInputLayout(std::vector<VertexAttributeDesc> descs) override;
		virtual ShaderHandle CreateShader(const ShaderDesc& d) override;
		virtual FrameBufferHandle CreateFrameBuffer() override;
		virtual PipelineStateHandle CreatePiplelineState(const PipelineDesc& d) override;
		virtual ComputePipelineState CreateComputePipelineState(const ComputePipelineDesc& d) override;
	private:
		EGLDisplay mEGLDisplay = EGL_NO_DISPLAY;
		EGLContext mEGLContext = EGL_NO_CONTEXT;
	};
}
}