#pragma once
#include<string>
namespace redtea
{
namespace device
{
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
	private:
		void Resize();
		std::string mTitle;
		size_t mWidth;
		size_t mHeight;
		void* mWindow = nullptr;
    };
}
}