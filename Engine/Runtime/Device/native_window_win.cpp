#include "native_window.h"
#include <SDL_syswm.h>

void* getNativeWindow(SDL_Window* sdlWindow) {
	SDL_SysWMinfo wmi;
	SDL_VERSION(&wmi.version);
	SDL_GetWindowWMInfo(sdlWindow, &wmi);
	HWND win = (HWND)wmi.info.win.window;
	return (void*)win;
}