//
// Created by Alex on 12/13/2020.
//

#pragma once

#include "InternalPointer.hpp"
#include "SDLWrapper.hpp"

namespace RPG {
	struct Bitmap {
		public:
			Bitmap(SDL_Surface* surface);
			uint16_t GetWidth() const;
			uint16_t GetHeight() const;
			void* GetPixelData() const;

		private:
			struct Internal;
			RPG::InternalPointer<Internal> internal;
	};
}


