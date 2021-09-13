#pragma once
#include "../Device/window.h"
namespace redtea {
    class RedteaApp
    {
    public:
        void Initialize();
        void Run();
        void Destroy();

        static RedteaApp& GetInstance();
		RedteaApp();
        RedteaApp(const RedteaApp& rhs) = delete;
        RedteaApp(RedteaApp&& rhs) = delete;
        RedteaApp& operator=(const RedteaApp& rhs) = delete;
        RedteaApp& operator=(RedteaApp&& rhs) = delete;
		inline device::RedteaWindow* GetWindow() { return mWindow; }
    private:
		device::RedteaWindow *mWindow;
        bool mDestroyed = false;
    };
}