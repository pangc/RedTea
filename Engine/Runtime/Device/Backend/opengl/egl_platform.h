#pragma once
#include "gl_platform.h"
#include "EGL/egl.h"
#include "EGL/eglext.h"

namespace redtea
{
namespace device
{
	class EGLPlatform : public GLPlatform
	{
	public:
		EGLPlatform() = default;
		static EGLPlatform* Create();
		virtual void Destroy() override;
		virtual void* GetWindow() override { return mNativeWindows; }
		virtual void SetWindow(void *p) override { mNativeWindows = p; }
		virtual bool InitContext() override;
		virtual SwapChainHandle CreateSwapChain(const SwapChainDesc& d) override;
		virtual void MakeCurrent(SwapChainHandle draw, SwapChainHandle read) override;
	private:
		EGLBoolean MakeCurrent(EGLSurface drawSurface, EGLSurface readSurface);
		EGLSurface CreateSurface(uint16_t width, uint16_t height, uint16_t msaa, bool isTransparent = false);

		EGLDisplay mEGLDisplay = EGL_NO_DISPLAY;
		EGLContext mEGLContext = EGL_NO_CONTEXT;

		EGLSurface mCurrentDrawSurface = EGL_NO_SURFACE;
		EGLSurface mCurrentReadSurface = EGL_NO_SURFACE;

		EGLConfig mEGLConfig = EGL_NO_CONFIG_KHR;
		EGLConfig mEGLTransparentConfig = EGL_NO_CONFIG_KHR;

		EGLSurface mEGLDummySurface = EGL_NO_SURFACE;

		//opengl shared context
		void* sharedContext = EGL_NO_CONTEXT;
		void* mNativeWindows;
	};
}
}