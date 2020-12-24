//
// Created by Alex on 12/13/2020.
//

#include "SDLWrapper.hpp"
#include "Platform.hpp"

#ifdef __EMSCRIPTEN__
	#include <emscripten.h>
#endif

namespace {
	bool ShouldDisplayFullScreen() {
		switch(RPG::GetCurrentPlatform()) {
			case RPG::Platform::iOS:
			case RPG::Platform::android:
				return true;
			default:
				return false;
		}
	}

	#ifdef __EMSCRIPTEN__
		RPG::WindowSize GetEmscriptenCanvasSize() {
			// For Emscripten targets we will invoke some Javascript
			// to find out the dimensions of the canvas in the HTML
			// document. Note that the 'width' and 'height' attributes
			// need to be set on the <canvas /> HTML element, like so:
			// <canvas id="canvas" width="600", height="360"></canvas>
			uint32_t width{static_cast<uint32_t>(EM_ASM_INT({
				return document.getElementById('canvas').width;
			}))};

			uint32_t height{static_cast<uint32_t>(EM_ASM_INT({
				return document.getElementById('canvas').height;
			}))};

			return RPG::WindowSize{width, height};
		}
	#endif
}

RPG::WindowSize RPG::SDL::GetWindowSize(SDL_Window* window) {
	#ifdef __EMSCRIPTEN__
		return ::GetEmscriptenCanvasSize();
	#else
		int width{0};
		int height{0};
		SDL_GetWindowSize(window, &width, &height);
		return RPG::WindowSize{static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
	#endif
}

RPG::WindowSize RPG::SDL::GetInitialWindowSize() {
	#ifdef __EMSCRIPTEN__
		return ::GetEmscriptenCanvasSize();
	#else
		const RPG::Platform platform{RPG::GetCurrentPlatform()};

		if (platform == RPG::Platform::iOS || platform == RPG::Platform::android) {
			// For mobile platforms we will fetch the full screen size.
			SDL_DisplayMode displayMode;
			SDL_GetDesktopDisplayMode(0, &displayMode);
			return RPG::WindowSize{static_cast<uint32_t>(displayMode.w), static_cast<uint32_t>(displayMode.h)};
		}

		// For other platforms we'll just show a fixed size window.
		return RPG::WindowSize{1280, 720};
	#endif
}

SDL_Window* RPG::SDL::CreateWindowFunc(const uint32_t& windowFlags) {
	RPG::WindowSize windowSize{RPG::SDL::GetInitialWindowSize()};

	//TODO: Change title to the application/game name somewhere in like a settings file
	SDL_Window* window{SDL_CreateWindow(
			"RPG-Engine",
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			windowSize.width, windowSize.height,
			windowFlags)};

	if (::ShouldDisplayFullScreen()) {
		SDL_SetWindowFullscreen(window, SDL_TRUE);
	}

	return window;
}