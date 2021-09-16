#include "gl_device.h"
#include "logger/logger.h"
#include "GLES3/gl3.h"
#include "egl_platform.h"

namespace redtea
{
namespace device
{
	GLSDevice::~GLSDevice()
	{
		if (platform)
		{
			platform->Destroy();
		}
	}

	bool GLSDevice::InitDevice(void* window)
	{
		platform = EGLPlatform::Create();
		platform->SetWindow(window);
		platform->InitContext();
		InitializeGlExtensions();
		return true;
	}

	SwapChainHandle GLSDevice::CreateSwapchain()
	{
		return SwapChainHandle();
	}

	HeapHandle GLSDevice::CreateHeap(const HeapDesc & d)
	{
		return HeapHandle();
	}

	TextureHandle GLSDevice::CreateTexture(const TextureDesc & d)
	{
		return TextureHandle();
	}

	SamplerStateHandle GLSDevice::CreateSampler(const SamplerDesc & d)
	{
		return SamplerStateHandle();
	}

	VertexBufferHandle GLSDevice::CreateVertexBuffer(const BufferDesc & d)
	{
		return VertexBufferHandle();
	}

	IndexBufferHandle GLSDevice::CreateIndexBuffer(const BufferDesc & d)
	{
		return IndexBufferHandle();
	}

	InputLayoutHandle GLSDevice::CreateInputLayout(std::vector<VertexAttributeDesc> descs)
	{
		return InputLayoutHandle();
	}

	ShaderHandle GLSDevice::CreateShader(const ShaderDesc & d)
	{
		return ShaderHandle();
	}

	FrameBufferHandle GLSDevice::CreateFrameBuffer()
	{
		return FrameBufferHandle();
	}

	PipelineStateHandle GLSDevice::CreatePiplelineState(const PipelineDesc & d)
	{
		return PipelineStateHandle();
	}

	ComputePipelineState GLSDevice::CreateComputePipelineState(const ComputePipelineDesc & d)
	{
		return ComputePipelineState();
	}

	void GLSDevice::InitializeGlExtensions()
	{
		GLint n;
		glGetIntegerv(GL_NUM_EXTENSIONS, &n);
		for (GLint i = 0; i < n; ++i) {
			const char * const extension = (const char*)glGetStringi(GL_EXTENSIONS, (GLuint)i);
			if (extension)
				glExtensions.insert(extension);
		}
	}
}
}