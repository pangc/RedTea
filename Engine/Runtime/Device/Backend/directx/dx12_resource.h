#pragma once
#include <dxgi.h>
#include "../../RHI/rhi.h"
#include <d3d12.h>
#include <dxgi1_6.h>

namespace redtea {
namespace device {

	class DXSwapChain : public ISwapChain
	{
		DXSwapChain(SwapChainDesc& desc);
	private:
		IDXGISwapChain1* mSwapChain;
	};

}
}