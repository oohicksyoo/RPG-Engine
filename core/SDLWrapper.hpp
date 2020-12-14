//
// Created by Alex on 12/13/2020.
//

#pragma once

#include <SDL.h>
#include "WindowSize.hpp"

#ifndef __EMSCRIPTEN__
	#include <SDL_vulkan.h>
#endif


namespace RPG::SDL {
	RPG::WindowSize GetInitialWindowSize();
	RPG::WindowSize GetWindowSize(SDL_Window* window);
	SDL_Window* CreateWindowFunc(const uint32_t& windowFlags);
}


