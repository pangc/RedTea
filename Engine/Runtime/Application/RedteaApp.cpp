#include "RedteaApp.h"
#include <logger/logger.h>

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
	LOGD("Initialize");
}


void RedteaApp::Destroy()
{
	mWindow->Destroy();
}

void RedteaApp::Run()
{
	while (true)
	{
	}
}