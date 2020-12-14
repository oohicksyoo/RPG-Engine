//
// Created by Alex on 12/13/2020.
//

#include "Bitmap.hpp"

using RPG::Bitmap;

struct Bitmap::Internal {
	SDL_Surface* surface;

	Internal(SDL_Surface* surface) : surface(surface) {}

	~Internal() {
		SDL_FreeSurface(surface);
	}
};

Bitmap::Bitmap(SDL_Surface* surface) : internal(RPG::MakeInternalPointer<Internal>(surface)) {}

uint16_t Bitmap::GetWidth() const {
	return static_cast<uint16_t>(internal->surface->w);
}

uint16_t Bitmap::GetHeight() const {
	return static_cast<uint16_t>(internal->surface->h);
}

void* Bitmap::GetPixelData() const {
	return internal->surface->pixels;
}