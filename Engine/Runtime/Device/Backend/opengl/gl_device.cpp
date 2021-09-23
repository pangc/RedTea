#include "gl_device.h"
#include "logger/logger.h"
#include "egl_platform.h"
#include "GLES3/gl3.h"

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

	SwapChainHandle GLSDevice::CreateSwapchain(const SwapChainDesc& d)
	{
		SwapChainHandle handle = platform->CreateSwapChain(d);
		return handle;
	}

	void GLSDevice::MakeCurrent(SwapChainHandle draw, SwapChainHandle read)
	{
		platform->MakeCurrent(draw, read);
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

	IndexBufferHandle GLSDevice::CreateIndexBuffer(const BufferDesc & d)
	{
		return IndexBufferHandle();
	}

	ShaderHandle GLSDevice::CreateShader(const ShaderDesc & d)
	{
		return ShaderHandle();
	}

	RenderPrimitiveHandle GLSDevice::CreateRenderPrimitive()
	{
		return RenderPrimitiveHandle();
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

	void GLSDevice::CreateBufferObject(BufferHandle handle, BufferDesc d, uint32_t size)
	{
		GLBuffer* buffer = new GLBuffer(d);
		glGenBuffers(1, &(buffer->id));
		glBufferData(GL_ARRAY_BUFFER, d.size, nullptr, GL_STATIC_DRAW);
		handle.Attach(buffer);
	}

	void GLSDevice::UpdateBufferObject(BufferHandle handle, uint32_t offset, BufferData && data)
	{
		GLBuffer* buffer = (GLBuffer*) handle.Get();
		glBindBuffer(buffer->target, buffer->id);
		ASSERT(offset + data.size <= buffer->GetDescription().size);
		glBufferSubData(GL_ARRAY_BUFFER, offset, data.size, data.buffer);
		// release data
	}

	void GLSDevice::CreateRenderPrimitiveObject(RenderPrimitiveHandle handle)
	{
		GLRenderPrimitive* primitive = (GLRenderPrimitive*)handle.Get();
		glGenVertexArrays(1, &primitive->vao);
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