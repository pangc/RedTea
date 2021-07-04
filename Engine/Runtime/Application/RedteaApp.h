#pragma once
#include "../Device/Window.h"
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
		device::RedteaWindow *mWindow;
    private:
        bool m_destroyed = false;
    };
}