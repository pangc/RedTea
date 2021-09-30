#include "dx12_resource.h"
namespace redtea {
namespace device{

DXSwapChain::DXSwapChain(IDXGISwapChain1* swapchain)
{
	mSwapChain.Attach(swapchain);
}

DXBuffer::DXBuffer(const BufferDesc &desc) : IBuffer(desc)
{
	
}

}
}
