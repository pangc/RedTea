#include "dx12_device.h"
#include <wrl/client.h>
#include <dxgi1_6.h>
#include "common.h"
using Microsoft::WRL::ComPtr;

namespace redtea {
namespace device{
#define LOGIfFailed(hr, msg) if(FAILED(hr)){LOGE(msg); return false;}

bool DX12Device::InitDevice(void* windows)
{
	auto hr = CreateDXGIFactory1(IID_PPV_ARGS(&mFactory));

	LOGIfFailed(hr, "DXGI factory creation failed.");

	// Find Adaptor
	ComPtr<IDXGIAdapter1> hardwareAdapter;
	FindAdaptor(mFactory, &hardwareAdapter);

	hr = D3D12CreateDevice(
		hardwareAdapter.Get(),
		D3D_FEATURE_LEVEL_12_0,
		IID_PPV_ARGS(&mDevice)
	);
	LOGIfFailed(hr, "DXGI create device failed.");


	// Init Command Queue
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	hr = mDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&mQueue));
	LOGIfFailed(hr, "DXGI create command queue failed.");
	return true;
}

SwapChainHandle DX12Device::CreateSwapchain(const SwapChainDesc & desc)
{
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.BufferCount = SWAPCHAIN_BUFFER_COUNT;
	swapChainDesc.Width = desc.width;
	swapChainDesc.Height = desc.height;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.SampleDesc.Count = 1;

	return SwapChainHandle();
}

void DX12Device::MakeCurrent(SwapChainHandle draw, SwapChainHandle read)
{
}

HeapHandle DX12Device::CreateHeap(const HeapDesc & d)
{
	return HeapHandle();
}

TextureHandle DX12Device::CreateTexture(const TextureDesc & d)
{
	return TextureHandle();
}

SamplerStateHandle DX12Device::CreateSampler(const SamplerDesc & d)
{
	return SamplerStateHandle();
}

IndexBufferHandle DX12Device::CreateIndexBuffer(const BufferDesc & d)
{
	return IndexBufferHandle();
}

ShaderHandle DX12Device::CreateShader(const ShaderDesc & d)
{
	return ShaderHandle();
}

RenderPrimitiveHandle DX12Device::CreateRenderPrimitive()
{
	return RenderPrimitiveHandle();
}

FrameBufferHandle DX12Device::CreateFrameBuffer()
{
	return FrameBufferHandle();
}

PipelineStateHandle DX12Device::CreatePiplelineState(const PipelineDesc & d)
{
	return PipelineStateHandle();
}

ComputePipelineState DX12Device::CreateComputePipelineState(const ComputePipelineDesc & d)
{
	return ComputePipelineState();
}

void DX12Device::FindAdaptor(IDXGIFactory1* pFactory, IDXGIAdapter1** ppAdapter)
{
	ComPtr<IDXGIAdapter1> hardwareAdapter;
	ComPtr<IDXGIAdapter1> adapter;
	ComPtr<IDXGIFactory6> factory6;

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

DX12Device::~DX12Device()
{
}

}
}