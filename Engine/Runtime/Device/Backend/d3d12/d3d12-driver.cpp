#include "./d3d12-driver.h"
#include <common.h>

namespace redtea {
namespace device {
	RefCountPtr<IDXGIAdapter> DXDriver::GetAdaptor()
	{
		RefCountPtr<IDXGIAdapter> targetAdapter;
		RefCountPtr<IDXGIFactory1> DXGIFactory;
		HRESULT hr = CreateDXGIFactory1(IID_PPV_ARGS(&DXGIFactory));
		LOGIfFailed(hr, "DXGI factory creation failed.");


		unsigned int adapterNo = 0;
		while (SUCCEEDED(hr))
		{
			RefCountPtr<IDXGIAdapter> pAdapter;
			hr = DXGIFactory->EnumAdapters(adapterNo, &pAdapter);

			if (SUCCEEDED(hr))
			{
				DXGI_ADAPTER_DESC aDesc;
				pAdapter->GetDesc(&aDesc);
				targetAdapter = pAdapter;

				// Check to see whether the adapter supports Direct3D 12, but don't create the
				// actual device yet.
				if (SUCCEEDED(D3D12CreateDevice(targetAdapter.Get(), D3D_FEATURE_LEVEL_12_0, _uuidof(ID3D12Device), nullptr)))
				{
					std::wstring aName = aDesc.Description;
					std::string name(aName.begin(), aName.end());
					LOGD_FORMAT("Find Adaptor:%s !", name.c_str());
					break;
				}
			}

			adapterNo++;
		}


		return targetAdapter;
	}

	bool DXDriver::InitContext(DeviceCreationParameters param)
	{
		if (!param.nativeWindow)
		{
			LOGD("Init Context failed,  nativeWindows is nullptr");
			return false;
		}
		m_hWnd = (HWND)param.nativeWindow;
		RECT clientRect;
		GetClientRect(m_hWnd, &clientRect);
		m_Width = clientRect.right - clientRect.left;
		m_Height = clientRect.bottom - clientRect.top;
		LOGD_FORMAT("Current Client size is %d, %d", m_Width, m_Height);

		// set adaptor
		RefCountPtr<IDXGIAdapter> targetAdapter = GetAdaptor();
		m_DxgiAdapter = targetAdapter;

		RefCountPtr<IDXGIFactory2> pDxgiFactory;
		HRESULT hr = CreateDXGIFactory2(0, IID_PPV_ARGS(&pDxgiFactory));

		RefCountPtr<IDXGIFactory5> pDxgiFactory5;
		if (SUCCEEDED(pDxgiFactory->QueryInterface(IID_PPV_ARGS(&pDxgiFactory5))))
		{
			BOOL supported = 0;
			if (SUCCEEDED(pDxgiFactory5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &supported, sizeof(supported))))
				m_TearingSupported = (supported != 0);
		}

		// create device
		hr = D3D12CreateDevice(
			m_DxgiAdapter,
			D3D_FEATURE_LEVEL_11_1,
			IID_PPV_ARGS(&m_Device12));
		LOGIfFailed(hr, "DXGI factory creation failed.");

		D3D12_COMMAND_QUEUE_DESC queueDesc;
		ZeroMemory(&queueDesc, sizeof(queueDesc));
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		queueDesc.NodeMask = 1;
		hr = m_Device12->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_GraphicsQueue));
		LOGIfFailed(hr, "Create Graphics Queue failed");
		m_GraphicsQueue->SetName(L"Graphics Queue");

		if (param.enableComputeQueue)
		{
			queueDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
			hr = m_Device12->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_ComputeQueue));
			LOGIfFailed(hr, "Create Compute Queue failed");
			m_ComputeQueue->SetName(L"Compute Queue");
		}

		if (param.enableCopyQueue)
		{
			queueDesc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
			hr = m_Device12->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_CopyQueue));
			LOGIfFailed(hr, "Create Copy Queue failed");
			m_CopyQueue->SetName(L"Copy Queue");
		}

		return true;
	}

	DeviceHandle DXDriver::CreateDevice(DeviceCreationParameters param)
	{
		bool succ = InitContext(param);
		if (!succ)
		{
			return DeviceHandle();
		}
		DeviceDesc deviceDesc;
		deviceDesc.pDevice = m_Device12;
		deviceDesc.pGraphicsCommandQueue = m_GraphicsQueue;
		deviceDesc.pComputeCommandQueue = m_ComputeQueue;
		deviceDesc.pCopyCommandQueue = m_CopyQueue;
		deviceDesc.nativeWindow = (HWND)param.nativeWindow;

		m_Device = createDevice(deviceDesc);
		return m_Device;
	}

	SwapChainHandle DXDriver::CreateSwapChain()
	{
		SwapChainDesc desc;
		desc.setWidth(m_Width)
			.setHeight(m_Height)
			.setSwapChainBufferCount(3)
			.setSwapChainSampleCount(1)
			.setSwapChainSampleQuality(0)
			.setSwapChainFormat(Format::SRGBA8_UNORM)
			.setAllowModeSwitch(true)
			.setAllowTearing(m_TearingSupported);
		m_SwapChain = m_Device->createSwapChain(desc);
		return m_SwapChain;
	}

	TextureHandle DXDriver::CreateTexture()
	{
		return TextureHandle();
	}

	void DXDriver::BeginFrame()
	{
	}
}
}