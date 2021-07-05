#pragma once
#include<string>
#include <functional>
#include <event.h>

namespace redtea
{
namespace device
{
	typedef  std::function<void(common::EventType, common::EventData)> EventCallback;
    class RedteaWindow
    {
	public:
		RedteaWindow() = delete;
		~RedteaWindow();
		RedteaWindow(size_t width, size_t height, std::string title);
		inline void SetTitle(std::string);
		std::string GetTitle() { return mTitle; }
		void SetWindowSize(size_t width, size_t height);
		void* GetNativeWindow();
		void Destroy();
		void PollEvents();
		float GetDeltaTime();
		inline void RegistEventCallback(EventCallback callback) { mCallback = callback; }
	private:
		void Resize();
		std::string mTitle;
		size_t mWidth;
		size_t mHeight;
		uint64_t mTime;
		EventCallback mCallback;
		void* mWindow = nullptr;
    };
}
}