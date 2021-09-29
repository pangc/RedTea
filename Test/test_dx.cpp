#include "common.h"
#include <gtest/gtest.h>
#include "../Engine/Runtime/Application/redtea_app.h"
#include "../Engine/Runtime/Device/Backend/directx/dx12_device.h"

TEST(DX12_TEST, dx_device)
{
	using namespace redtea;
	RedteaApp& app = RedteaApp::GetInstance();
	app.Initialize();

	// init gl
	auto device = new device::DX12Device();
	device->InitDevice(app.GetWindow()->GetNativeWindow());
}