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

	void RedteaWindow::Resize()
	{
		SDL_SetWindowSize((SDL_Window*)mWindow, mWidth, mHeight);
	}
}
}