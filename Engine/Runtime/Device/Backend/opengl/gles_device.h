#pragma once
#include "../../RHI/device.h"
#include "EGL/egl.h"
#include "EGL/eglext.h"
#include <unordered_set>

namespace redtea
{
namespace device
{
	class ESDevice : public IDevice
	{
	public:
		virtual bool InitDevice(void* windows) override;
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
		void InitializeGlExtensions();
		EGLBoolean MakeCurrent(EGLSurface drawSurface, EGLSurface readSurface);

		EGLDisplay mEGLDisplay = EGL_NO_DISPLAY;
		EGLContext mEGLContext = EGL_NO_CONTEXT;

		EGLSurface mCurrentDrawSurface = EGL_NO_SURFACE;
		EGLSurface mCurrentReadSurface = EGL_NO_SURFACE;

		EGLConfig mEGLConfig = EGL_NO_CONFIG_KHR;
		EGLConfig mEGLTransparentConfig = EGL_NO_CONFIG_KHR;

		EGLSurface mEGLDummySurface = EGL_NO_SURFACE;

		//opengl shared context
		void* sharedContext = EGL_NO_CONTEXT;
		std::unordered_set<std::string> glExtensions;
	};
}
}