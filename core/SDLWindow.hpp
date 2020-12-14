//
// Created by Alex on 12/13/2020.
//

#pragma once

#include "InternalPointer.hpp"
#include "SDLWrapper.hpp"

namespace RPG {
	struct SDLWindow {
			SDLWindow(const uint32_t& windowFlags);
			SDL_Window* GetWindow() const;

		private:
			struct Internal;
			RPG::InternalPointer<Internal> internal;
	};
}


