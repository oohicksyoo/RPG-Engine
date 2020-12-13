//
// Created by Alex on 12/13/2020.
//

#include "Engine.hpp"
#include "Log.hpp"
#include <SDL_image.h>

using RPG::Engine;

struct Engine::Internal {
	const std::string logTag = "Engine";

	Internal() {}

	void Run() {
		RPG::Log(logTag, "Starting engine up");
		SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
		RPG::Log(logTag, "SDL2 initialized successfully");

		if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
			throw std::runtime_error("[" + logTag + "] Could not initialize SDL2_Image");
		}
		RPG::Log(logTag, "SDL2_image initialized successfully with PNG support");


	}

	~Internal() {
		RPG::Log(logTag, "Cleaning up Engine");
		SDL_Quit();
	}
};

Engine::Engine() : internal(RPG::MakeInternalPointer<Internal>()) {}

void Engine::Run() {
	internal->Run();
}
