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
	device->InitDevice(app.GetWindow());
	app.Run();
}