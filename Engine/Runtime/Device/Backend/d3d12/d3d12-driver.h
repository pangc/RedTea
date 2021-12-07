#pragma once

#include "d3d12.h"
#include <dxgi1_6.h>

namespace redtea {
namespace device {

	struct DeviceCreationParameters
	{
		void* nativeWindow = nullptr;
		bool enableComputeQueue = false;
		bool enableCopyQueue = false;
	};

	class Driver
	{
	public:
		virtual void BeginFrame() = 0;
		virtual DeviceHandle CreateDevice(DeviceCreationParameters param) = 0;
		virtual SwapChainHandle CreateSwapChain() = 0;
	};

	class DXDriver : public Driver
	{
	private:
		bool InitContext(DeviceCreationParameters param);

		RefCountPtr<ID3D12Device>                   m_Device12;
		RefCountPtr<ID3D12CommandQueue>             m_GraphicsQueue;
		RefCountPtr<ID3D12CommandQueue>             m_ComputeQueue;
		RefCountPtr<ID3D12CommandQueue>             m_CopyQueue;
		RefCountPtr<IDXGIAdapter>                   m_DxgiAdapter;
		RefCountPtr<ID3D12Fence>                    m_FrameFence;
		std::vector<HANDLE>                         m_FrameFenceEvents;
		UINT64                                      m_FrameCount = 1;

		HWND                                        m_hWnd = nullptr;
		bool										m_TearingSupported; // Variable rate fresh need tearing support
	
		UINT										m_Width;
		UINT										m_Height;
		
		// RHI
		DeviceHandle								m_Device = nullptr;
		SwapChainHandle								m_SwapChain = nullptr;
	public:
		static RefCountPtr<IDXGIAdapter> GetAdaptor();

		DeviceHandle CreateDevice(DeviceCreationParameters param) override;
		SwapChainHandle CreateSwapChain() override;
		TextureHandle CreateTexture();
		void BeginFrame() override;
	};
}
}