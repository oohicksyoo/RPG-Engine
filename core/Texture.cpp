//
// Created by Alex on 12/13/2020.
//

#include "Texture.hpp"

using RPG::Texture;

struct Texture::Internal {
	SDL_Surface* surface;
	std::string path;

	Internal(SDL_Surface* surface, std::string path) : surface(surface), path(path) {}

	~Internal() {
		SDL_FreeSurface(surface);
	}
};

Texture::Texture(SDL_Surface* surface, std::string path) : internal(RPG::MakeInternalPointer<Internal>(surface, path)) {}

uint16_t Texture::GetWidth() const {
	return static_cast<uint16_t>(internal->surface->w);
}

uint16_t Texture::GetHeight() const {
	return static_cast<uint16_t>(internal->surface->h);
}

void* Texture::GetPixelData() const {
	return internal->surface->pixels;
}

std::string Texture::GetPath() const {
    return internal->path;
}