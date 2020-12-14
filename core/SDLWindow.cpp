//
// Created by Alex on 12/13/2020.
//

#include "SDLWindow.hpp"

using RPG::SDLWindow;

struct SDLWindow::Internal {
	SDL_Window* window;

	Internal(const uint32_t& windowFlags) : window(RPG::SDL::CreateWindowFunc(windowFlags)) {}

	~Internal() {
		SDL_DestroyWindow(window);
	}
};

SDLWindow::SDLWindow(const uint32_t& windowFlags) : internal(RPG::MakeInternalPointer<Internal>(windowFlags)) {}

SDL_Window* SDLWindow::GetWindow() const {
	return internal->window;
}