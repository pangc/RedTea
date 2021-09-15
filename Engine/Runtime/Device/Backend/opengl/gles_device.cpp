#include "gles_device.h"
#include "logger/logger.h"
#include "GLES3/gl3.h"

namespace redtea
{
namespace device
{
	bool ESDevice::InitDevice(void* window)
	{
		mEGLDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
		EGLint major, minor;
		EGLBoolean initialized = eglInitialize(mEGLDisplay, &major, &minor);

		EGLint configsCount;

		EGLint configAttribs[] = {
				EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT_KHR,        //  0
				EGL_RED_SIZE,    8,                                 //  2
				EGL_GREEN_SIZE,  8,                                 //  4
				EGL_BLUE_SIZE,   8,                                 //  6
				EGL_ALPHA_SIZE,  0,                                 //  8 : reserved to set ALPHA_SIZE below
				EGL_DEPTH_SIZE, 24,                                 // 10
				EGL_STENCIL_SIZE,8,									// 12
				EGL_NONE                                            // 14
		};

		EGLint contextAttribs[] = {
				EGL_CONTEXT_CLIENT_VERSION, 3,
				EGL_NONE, EGL_NONE, // reserved for EGL_CONTEXT_OPENGL_NO_ERROR_KHR below
				EGL_NONE
		};

		EGLint pbufferAttribs[] = {
				EGL_WIDTH,  1,
				EGL_HEIGHT, 1,
				EGL_NONE
		};

		if (!eglChooseConfig(mEGLDisplay, configAttribs, &mEGLConfig, 1, &configsCount))
		{
			LOGE("Can't create opaque config");
			return false;
		}

		configAttribs[8] = EGL_ALPHA_SIZE;
		configAttribs[9] = 8;
		if (!eglChooseConfig(mEGLDisplay, configAttribs, &mEGLTransparentConfig, 1, &configsCount) ||
			(configAttribs[13] == EGL_DONT_CARE && configsCount == 0))
		{
			LOGE("Can't create transparent config");
			return false;
		}

		// 创建一个空surface才能获取extension信息
		mEGLDummySurface = eglCreatePbufferSurface(mEGLDisplay, mEGLTransparentConfig, pbufferAttribs);
		if (mEGLDummySurface == EGL_NO_SURFACE) {
			LOGE("Can't create pbuffer");
			return false;;
		}

		mEGLContext = eglCreateContext(mEGLDisplay, mEGLConfig, (EGLContext)sharedContext, contextAttribs);

		if (!MakeCurrent(mEGLDummySurface, mEGLDummySurface)) {
			// eglMakeCurrent failed
			LOGE("Can't make dummy surface");
			eglDestroySurface(mEGLDisplay, mEGLDummySurface);
			mEGLDummySurface = EGL_NO_SURFACE;
			return false;
		}

		InitializeGlExtensions();
		return true;
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

	void ESDevice::InitializeGlExtensions()
	{
		GLint n;
		glGetIntegerv(GL_NUM_EXTENSIONS, &n);
		for (GLint i = 0; i < n; ++i) {
			const char * const extension = (const char*)glGetStringi(GL_EXTENSIONS, (GLuint)i);
			if(extension)
				glExtensions.insert(extension);
		}
	}
	EGLBoolean ESDevice::MakeCurrent(EGLSurface drawSurface, EGLSurface readSurface)
	{
		if (UNLIKELY((drawSurface != mCurrentDrawSurface || readSurface != mCurrentReadSurface)))
		{
			mCurrentDrawSurface = drawSurface;
			mCurrentReadSurface = readSurface;
			return eglMakeCurrent(mEGLDisplay, drawSurface, readSurface, mEGLContext);
		}
		return EGL_TRUE;
	}
}
}