#pragma once

#include "d3d12.h"
#include <dxgi1_6.h>

namespace redtea {
namespace device {


	class Driver
	{

	};

	struct DeviceCreationParameters
	{
		bool enableComputeQueue = false;
		bool enableCopyQueue = false;
	};

	class DXDriver : public Driver
	{
		RefCountPtr<ID3D12Device>                   m_Device12;
		RefCountPtr<ID3D12CommandQueue>             m_GraphicsQueue;
		RefCountPtr<ID3D12CommandQueue>             m_ComputeQueue;
		RefCountPtr<ID3D12CommandQueue>             m_CopyQueue;
		RefCountPtr<IDXGISwapChain3>                m_SwapChain;
		DXGI_SWAP_CHAIN_DESC1                       m_SwapChainDesc{};
		DXGI_SWAP_CHAIN_FULLSCREEN_DESC             m_FullScreenDesc{};
		RefCountPtr<IDXGIAdapter>                   m_DxgiAdapter;
		std::vector<RefCountPtr<ID3D12Resource>>    m_SwapChainBuffers;
		std::vector<TextureHandle>					m_RhiSwapChainBuffers;
		RefCountPtr<ID3D12Fence>                    m_FrameFence;
		std::vector<HANDLE>                         m_FrameFenceEvents;

		UINT64                                      m_FrameCount = 1;

		HWND                                        m_hWnd = nullptr;
		bool										m_TearingSupported; // Variable rate fresh need tearing support
	
		UINT										m_Width;
		UINT										m_Height;
	public:
		static RefCountPtr<IDXGIAdapter> GetAdaptor();
		bool InitContext(void* window, DeviceCreationParameters param);
		DeviceHandle CreateDevice();
	};
}
}