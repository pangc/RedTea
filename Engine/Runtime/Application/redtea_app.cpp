#include "redtea_app.h"
#include <logger/logger.h>
#include <chrono>
#include <thread>

using namespace redtea;


RedteaApp& RedteaApp::GetInstance()
{
	static RedteaApp sharedApplication;
	return sharedApplication;
}

RedteaApp::RedteaApp()
{

}

void RedteaApp::Initialize()
{
	mWindow = new device::RedteaWindow(1080, 750, "Red Tea Engine");
	device::EventCallback callback = [&](common::EventType type, common::EventData data) -> void {
		switch (type)
		{
		case common::EventType::QUIT:
			mDestroyed = true;
			break;
		case common::EventType::KEY_DOWN:
		case common::EventType::KEY_UP:
			common::rlog.d << "key is:" << data.key << redtea::common::endl;
			break;
		case common::EventType::MOUSE_UP:
		case common::EventType::MOUSE_DOWN:
			break;
		case common::EventType::WINDOWS_RESIZE:
			mWindow->Resize();
			break;
		}
	};
	mWindow->RegistEventCallback(callback);
	LOGD("Initialize");
}


void RedteaApp::Destroy()
{
	mWindow->Destroy();
}

void RedteaApp::Run()
{
	// ����60fps
	const float fps = 1000.0 / 60;
	while (!mDestroyed)
	{
		const float dt = mWindow->GetDeltaTime();
		const int diff = int(fps - dt);
		if (diff > 0)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(diff));
		}

		mWindow->PollEvents();
	}
}