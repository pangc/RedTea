#include "dx12_device.h"
#include <wrl/client.h>
#include <dxgi1_6.h>
#include "common.h"
using Microsoft::WRL::ComPtr;

namespace redtea {
namespace device{

bool DX12Device::InitDevice(void* windows)
{
	ComPtr<IDXGIFactory4> dxgiFactory;
	auto hr = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));

	if (FAILED(hr)) {
		LOGE("DXGI factory creation failed.");
		return false;
	}

	// Find Adaptor
	ComPtr<IDXGIAdapter1> hardwareAdapter;
	FindAdaptor(dxgiFactory.Get(), &hardwareAdapter);

	hr = D3D12CreateDevice(
		hardwareAdapter.Get(),
		D3D_FEATURE_LEVEL_12_0,
		IID_PPV_ARGS(&mDevice)
	);

	if (FAILED(hr)) {
		LOGE("DXGI create device failed.");
		return false;
	}

	return true;
}

SwapChainHandle DX12Device::CreateSwapchain(const SwapChainDesc & d)
{
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