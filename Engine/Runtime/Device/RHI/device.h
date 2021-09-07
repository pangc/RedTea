#pragma once
#include "resource.h"
#include "rhi.h"

namespace redtea
{
namespace device
{
	class IDevice : public IResource
	{
	public:
		virtual SwapChainHandle CreateSwapchain() = 0;
	};
}
}