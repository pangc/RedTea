#pragma once
#include "gl_resource.h"

namespace redtea
{
namespace device
{
	class GLPlatform
	{
	public:
		virtual void* GetWindow() = 0;
		virtual void SetWindow(void *p) = 0;

		virtual void Destroy() = 0;
		virtual ~GLPlatform() {}
		virtual bool InitContext() = 0;
		virtual void MakeCurrent(SwapChainHandle draw, SwapChainHandle read) = 0;
		virtual SwapChainHandle CreateSwapChain(const SwapChainDesc& d) = 0;
	};
}
}