#include "gles_device.h"

namespace redtea
{
namespace device
{
	void ESDevice::InitDevice(void* window)
	{
		mEGLDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
		EGLint major, minor;
		EGLBoolean initialized = eglInitialize(mEGLDisplay, &major, &minor);
	}

	SwapChainHandle ESDevice::CreateSwapchain()
	{
		return SwapChainHandle();
	}

	HeapHandle ESDevice::CreateHeap(const HeapDesc & d)
	{
		return HeapHandle();
	}
	TextureHandle ESDevice::CreateTexture(const TextureDesc & d)
	{
		return TextureHandle();
	}
	SamplerStateHandle ESDevice::CreateSampler(const SamplerDesc & d)
	{
		return SamplerStateHandle();
	}
	VertexBufferHandle ESDevice::CreateVertexBuffer(const BufferDesc & d)
	{
		return VertexBufferHandle();
	}
	IndexBufferHandle ESDevice::CreateIndexBuffer(const BufferDesc & d)
	{
		return IndexBufferHandle();
	}
	InputLayoutHandle ESDevice::CreateInputLayout(std::vector<VertexAttributeDesc> descs)
	{
		return InputLayoutHandle();
	}
	ShaderHandle ESDevice::CreateShader(const ShaderDesc & d)
	{
		return ShaderHandle();
	}
	FrameBufferHandle ESDevice::CreateFrameBuffer()
	{
		return FrameBufferHandle();
	}
	PipelineStateHandle ESDevice::CreatePiplelineState(const PipelineDesc & d)
	{
		return PipelineStateHandle();
	}
	ComputePipelineState ESDevice::CreateComputePipelineState(const ComputePipelineDesc & d)
	{
		return ComputePipelineState();
	}
}
}