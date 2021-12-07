#include "common.h"
#include <gtest/gtest.h>
#include "../Engine/Runtime/Application/redtea_app.h"
#include "../Engine/Runtime/Device/Backend/d3d12/d3d12-driver.h"

TEST(DX12_TEST, dx_device)
{
	using namespace redtea;
	RedteaApp& app = RedteaApp::GetInstance();
	app.Initialize();
	auto nativeWindow = app.GetWindow()->GetNativeWindow();
	auto driver = new redtea::device::DXDriver();
	device::DeviceCreationParameters param;
	param.nativeWindow = nativeWindow;
	param.enableComputeQueue = false;
	param.enableCopyQueue = false;

	auto device = driver->CreateDevice(param);
	auto swapchain = driver->CreateSwapChain();
	driver->BeginFrame();
	swapchain->Present();
	app.Run();
}