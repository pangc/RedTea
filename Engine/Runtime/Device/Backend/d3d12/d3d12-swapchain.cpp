#include "d3d12-backend.h"

namespace redtea
{
namespace device
{
	SwapChain::SwapChain(const SwapChainDesc& d, class Device* device)
	{
		m_Device = device;
		HWND hWnd = (HWND)device->getNativeWindow();
		auto queue = device->getQueue(CommandQueue::Graphics);

		desc = d;
		m_FullScreenDesc = {};
		m_FullScreenDesc.RefreshRate.Numerator = desc.refreshRate;
		m_FullScreenDesc.RefreshRate.Denominator = 1;
		m_FullScreenDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;
		m_FullScreenDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		m_FullScreenDesc.Windowed = true;

		ZeroMemory(&m_SwapChainDesc, sizeof(m_SwapChainDesc));
		m_SwapChainDesc.Width = desc.width;
		m_SwapChainDesc.Height = desc.height;
		m_SwapChainDesc.SampleDesc.Count = desc.swapChainSampleCount;
		m_SwapChainDesc.SampleDesc.Quality = 0;
		m_SwapChainDesc.BufferUsage = DXGI_USAGE_SHADER_INPUT | DXGI_USAGE_RENDER_TARGET_OUTPUT;
		m_SwapChainDesc.BufferCount = desc.swapChainBufferCount;
		m_SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		m_SwapChainDesc.Flags = desc.allowModeSwitch ? DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH : 0;

		switch (desc.swapChainFormat)  // NOLINT(clang-diagnostic-switch-enum)
		{
		case Format::SRGBA8_UNORM:
			m_SwapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			break;
		case Format::SBGRA8_UNORM:
			m_SwapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
			break;
		default:
			m_SwapChainDesc.Format = convertFormat(desc.swapChainFormat);
			break;
		}

		if (desc.allowTearing)
		{
			m_SwapChainDesc.Flags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
		}

		RefCountPtr<IDXGIFactory2> pDxgiFactory;
		HRESULT hr = CreateDXGIFactory2(0, IID_PPV_ARGS(&pDxgiFactory));

		RefCountPtr<IDXGISwapChain1> pSwapChain1;
		hr = pDxgiFactory->CreateSwapChainForHwnd(queue->queue, hWnd, &m_SwapChainDesc, &m_FullScreenDesc, nullptr, &pSwapChain1);
		
		if (FAILED(hr))
		{
			LOGD("Create swapchain for hwnd failed");
			return;
		}
		hr = pSwapChain1->QueryInterface(IID_PPV_ARGS(&m_SwapChain));

		if (FAILED(hr))
		{
			LOGD("QueryInterface for swapchain failed");
			return;
		}

		if (FAILED(hr))
		{
			LOGD("CreateFence for swapchain failed");
			return;
		}

		CreateSwapChainBuffer();
	}

	SwapChain::~SwapChain()
	{
	}

	bool SwapChain::CreateSwapChainBuffer()
	{
		m_SwapChainBuffers.resize(m_SwapChainDesc.BufferCount);
		m_RhiSwapChainBuffers.resize(m_SwapChainDesc.BufferCount);

		for (UINT n = 0; n < m_SwapChainDesc.BufferCount; n++)
		{
			const HRESULT hr = m_SwapChain->GetBuffer(n, IID_PPV_ARGS(&m_SwapChainBuffers[n]));
			LOGIfFailed(hr, "Get swapchain buffer failed");

			TextureDesc textureDesc;
			textureDesc.width = desc.width;
			textureDesc.height = desc.height;
			textureDesc.sampleCount = desc.swapChainSampleCount;
			textureDesc.sampleQuality = desc.swapChainSampleQuality;
			textureDesc.format = desc.swapChainFormat;
			textureDesc.debugName = "SwapChainBuffer";
			textureDesc.isRenderTarget = true;
			textureDesc.isUAV = false;
			textureDesc.initialState = ResourceStates::Present;
			textureDesc.keepInitialState = true;

			m_RhiSwapChainBuffers[n] = m_Device->createHandleForNativeTexture(ObjectTypes::D3D12_Resource, Object(m_SwapChainBuffers[n]), textureDesc);
		}

		for(UINT bufferIndex = 0; bufferIndex < m_SwapChainDesc.BufferCount; bufferIndex++)
		{
			m_FrameFenceEvents.push_back( CreateEvent(nullptr, false, true, NULL) );
		}

		return true;
	}

	bool SwapChain::ReleaseSwapChainBuffer()
	{
		if(!m_Device)
		{
			LOGD("deivce is nullptr in SwapChain::ReleaseSwapChainBuffer");
			return false;
		}
		 // Make sure that all frames have finished rendering
		m_Device->waitForIdle();

		// Release all in-flight references to the render targets
		m_Device->runGarbageCollection();

		// Set the events so that WaitForSingleObject in OneFrame will not hang later
		for(auto e : m_FrameFenceEvents)
			SetEvent(e);
		m_SwapChainBuffers.clear();
		m_RhiSwapChainBuffers.clear();
		return true;
	}

	TextureHandle SwapChain::GetCurrentBackendBuffer()
	{
		return m_RhiSwapChainBuffers[m_SwapChain->GetCurrentBackBufferIndex()];
	}

	bool SwapChain::Resize()
	{
		ReleaseSwapChainBuffer();
		if (!m_Device)
			return false;

		if (!m_SwapChain)
			return false;

		const HRESULT hr = m_SwapChain->ResizeBuffers(desc.swapChainBufferCount,
                                            desc.width,
                                            desc.height,
                                            m_SwapChainDesc.Format,
                                            m_SwapChainDesc.Flags);
		LOGIfFailed(hr, "SwapChain resize buffer failed");

		bool ret = CreateSwapChainBuffer();
		if(!ret)
		{
			LOGD("CreateSwapChainBuffer failed");
			return false;
		}
		return true;
	}

	void SwapChain::Present()
	{
		UINT presentFlags = 0;
		if (!desc.vsyncEnabled && m_FullScreenDesc.Windowed && desc.allowTearing)
			presentFlags |= DXGI_PRESENT_ALLOW_TEARING;
		m_SwapChain->Present(desc.vsyncEnabled ? 1 : 0, presentFlags);
	}

	HANDLE SwapChain::GetCurrentFrameFenceEvent()
	{
		auto bufferIndex = m_SwapChain->GetCurrentBackBufferIndex();
		return m_FrameFenceEvents[bufferIndex];
	}
}
}