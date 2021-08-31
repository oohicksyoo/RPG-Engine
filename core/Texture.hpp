//
// Created by Alex on 12/13/2020.
//

#pragma once

#include "InternalPointer.hpp"
#include "SDLWrapper.hpp"
#include <string>

namespace RPG {
	struct Texture {
		public:
			Texture(SDL_Surface* surface, std::string path);
			uint16_t GetWidth() const;
			uint16_t GetHeight() const;
			void* GetPixelData() const;
			std::string GetPath() const;

		private:
			struct Internal;
			RPG::InternalPointer<Internal> internal;
	};
}


