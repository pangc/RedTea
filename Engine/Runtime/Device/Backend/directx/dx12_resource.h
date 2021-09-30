#pragma once
#include <dxgi.h>
#include "../../RHI/rhi.h"
#include <d3d12.h>
#include <dxgi1_6.h>

namespace redtea {
namespace device {

class DXSwapChain : public ISwapChain
{
public:
	DXSwapChain(IDXGISwapChain1* swapchain);
	virtual void Resize(uint16_t width, uint16_t height) override {};
private:
	RefCountPtr<IDXGISwapChain1> mSwapChain;
};

class DXBuffer : public IBuffer
{
public:
	DXBuffer(const BufferDesc &d);
	RefCountPtr<ID3D12Resource> resource;
	D3D12_RESOURCE_DESC resourceDesc{};
};

}
}