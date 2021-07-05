#include "Window.h"
#include<SDL.h>
namespace redtea
{
namespace device
{
	RedteaWindow::~RedteaWindow()
	{
		SDL_DestroyWindow((SDL_Window*)mWindow);
		SDL_Quit();
	}

	RedteaWindow::RedteaWindow(size_t width, size_t height, std::string title)
	{
		SDL_Init(SDL_INIT_EVENTS);
		const int x = SDL_WINDOWPOS_CENTERED;
		const int y = SDL_WINDOWPOS_CENTERED;
		uint32_t windowFlags = SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI;
		// 支持窗口Size变化
		windowFlags |= SDL_WINDOW_RESIZABLE;

		mWindow = SDL_CreateWindow(title.c_str(), x, y, (int)width, (int)height, windowFlags);
		mWidth = width;
		mHeight = height;
		mTitle = title;
	}
	
	void RedteaWindow::SetTitle(std::string title)
	{
		mTitle = title;
		SDL_SetWindowTitle((SDL_Window*)mWindow, title.c_str());
	}

	void RedteaWindow::SetWindowSize(size_t width, size_t height)
	{
		mWidth = width;
		mHeight = height;
	}

	void * RedteaWindow::GetNativeWindow()
	{
		return nullptr;
	}

	void RedteaWindow::Destroy()
	{
		delete this;
	}

	void RedteaWindow::PollEvents()
	{
		// 限制一帧里监听的事件数量
		constexpr size_t MaxEvent = 16;
		SDL_Event events[MaxEvent];
		int nevents = 0;
		while (nevents < MaxEvent && SDL_PollEvent(&events[nevents]) != 0) {
			nevents++;
		}
		if (mCallback)
		{
			for (int i = 0; i < nevents; i++)
			{
				const SDL_Event& event = events[i];
				// 分发事件
				common::EventData data;
				data.wheel = 0;
				common::EventType type = common::EventType::UNKNOW;
				switch (event.type)
				{
				case SDL_QUIT:
					type = common::EventType::QUIT;
					break;
				case SDL_KEYDOWN:
					type = common::EventType::KEY_DOWN;
					if (event.key.keysym.scancode < common::KeyType::SCANCODE_COUNT)
					{
						// SCANE CODE 对齐
						data.key = (common::KeyType)event.key.keysym.scancode;
					}
					break;
				case SDL_KEYUP:
					type = common::EventType::KEY_UP;
					if (event.key.keysym.scancode < common::KeyType::SCANCODE_COUNT)
					{
						// SCANE CODE 对齐
						data.key = (common::KeyType)event.key.keysym.scancode;
					}
					break;
				case SDL_MOUSEWHEEL:
					type = common::EventType::MOUSE_WHEEEL;
					data.wheel = event.wheel.y;
					break;
				case SDL_MOUSEBUTTONDOWN:
					type = common::EventType::MOUSE_DOWN;
					data.button.x = event.button.x;
					data.button.y = event.button.y;
					data.button.type = (common::ButtonType)event.button.button;
					break;
				case SDL_MOUSEBUTTONUP:
					type = common::EventType::MOUSE_UP;
					data.button.x = event.button.x;
					data.button.y = event.button.y;
					data.button.type = (common::ButtonType)event.button.button;
					break;
				case SDL_MOUSEMOTION:
					type = common::EventType::MOUSE_MOTION;
					data.motion.x = event.button.x;
					data.motion.y = event.button.y;
					break;
				default:
					break;
				}
				mCallback(type, data);
			}
		}
	}

	float RedteaWindow::GetDeltaTime()
	{
		// 计算Time Step
		static uint64_t frequency = SDL_GetPerformanceFrequency();
		uint64_t now = SDL_GetPerformanceCounter();
		const float timeStep = mTime > 0 ? (float)((double)(now - mTime) * 1000 / frequency) :
			(float)(1000.0f / 60.0f);
		mTime = now;
		return timeStep;
	}

	void RedteaWindow::Resize()
	{
		SDL_SetWindowSize((SDL_Window*)mWindow, mWidth, mHeight);
	}
}
}