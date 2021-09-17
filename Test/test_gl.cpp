#include <gtest/gtest.h>
#include <string>
#include "../Engine/Runtime/Application/redtea_app.h"
#include "../Engine/Runtime/Device/Backend/opengl/gl_device.h"

TEST(GL, CreateWindows)
{
	using namespace redtea;
	RedteaApp& app = RedteaApp::GetInstance();
	app.Initialize();
	
	// init gl
	auto device = new device::GLSDevice();
	device->InitDevice(app.GetWindow()->GetNativeWindow());
	device::SwapChainDesc desc;
	desc.SetWidth(512)
		.SetHeight(512)
		.SetTransparent(false)
		.SetMsaaSample(4);
	auto swapchain = device->CreateSwapchain(desc);
	device->MakeCurrent(swapchain, swapchain);
	app.Run();
}