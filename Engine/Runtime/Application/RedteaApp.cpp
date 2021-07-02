#include "RedteaApp.h"
#include <SDL.h>
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
	LOGD("Initialize");
}


void RedteaApp::Destroy()
{

}

void RedteaApp::Run()
{

}

void RedteaApp::InitSDL()
{
	if (SDL_Init(SDL_INIT_EVENTS) == 0)
	{
		// 
	}
}
