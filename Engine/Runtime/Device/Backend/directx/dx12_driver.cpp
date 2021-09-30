#include "dx12_driver.h"
#include <dxgi1_6.h>
#include "common.h"
#include "dx12_resource.h"

#define LOGIfFailedReturn(hr, msg, ret) if(FAILED(hr)){LOGE(msg); return ret;}

namespace redtea
{
namespace device
{
bool DX12Driver::InitContext(void* window)
{
    mContext.nativeWindow = window;
    auto hr = CreateDXGIFactory1(IID_PPV_ARGS(&mContext.factory));

    LOGIfFailedReturn(hr, "DXGI factory creation failed.", false);

    // Find Adaptor
    RefCountPtr<IDXGIAdapter1> hardwareAdapter;
    FindAdaptor(mContext.factory.Get(), &hardwareAdapter);

    hr = D3D12CreateDevice(
        hardwareAdapter.Get(),
        D3D_FEATURE_LEVEL_12_0,
        IID_PPV_ARGS(&mContext.device)
    );
    LOGIfFailedReturn(hr, "DXGI create device failed.", false);


    // Init Command Queue
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

    hr = mContext.device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&mContext.commandQueue));
    LOGIfFailedReturn(hr, "DXGI create command queue failed.", false);

}

void DX12Driver::CreateSwapchain(SwapChainHandle &handle, const SwapChainDesc& desc)
{
    IDXGISwapChain1* swapChain;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.BufferCount = SWAPCHAIN_BUFFER_COUNT;
	swapChainDesc.Width = desc.width;
	swapChainDesc.Height = desc.height;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.SampleDesc.Count = 1;

	auto hr = mContext.factory->CreateSwapChainForHwnd(mContext.commandQueue, (HWND)mContext.nativeWindow, &swapChainDesc, nullptr, nullptr, &swapChain);
    LOGIfFailedReturn(hr, "DXGI create swapchain failed.");

    auto swap = new DXSwapChain(swapChain);
    handle.Attach(swap);
}

void DX12Driver::CreateBuffer(BufferHandle &handle, const BufferDesc& desc)
{
    DXBuffer* buffer = new DXBuffer(desc);
    D3D12_RESOURCE_DESC& resourceDesc = buffer->resourceDesc;
}

void DX12Driver::FindAdaptor(IDXGIFactory1* pFactory, IDXGIAdapter1** ppAdapter)
{
    
	RefCountPtr<IDXGIFactory6> factory6;
    RefCountPtr<IDXGIAdapter1> adapter;

	if (SUCCEEDED(pFactory->QueryInterface(IID_PPV_ARGS(&factory6))))
	{
		for (
			UINT adapterIndex = 0;
			SUCCEEDED(factory6->EnumAdapterByGpuPreference(adapterIndex, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter)));
			++adapterIndex)
		{
			DXGI_ADAPTER_DESC1 desc;
			adapter->GetDesc1(&desc);

			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				// Don't select the Basic Render Driver adapter.
				// If you want a software adapter, pass in "/warp" on the command line.
				continue;
			}

			// Check to see whether the adapter supports Direct3D 12, but don't create the
			// actual device yet.
			if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, _uuidof(ID3D12Device), nullptr)))
			{
				break;
			}
		}
	}
}

}
}