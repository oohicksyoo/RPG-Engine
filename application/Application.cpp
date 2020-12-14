//
// Created by Alex on 12/13/2020.
//

#ifdef __EMSCRIPTEN__
	#include <emscripten.h>
#endif

#include "Application.hpp"
#include "../core/SDLWrapper.hpp"

using RPG::Application;

namespace {
	#ifdef EMSCRIPTEN
		void EmscriptenMainLoop(RPG::Application* application) {
			application->RunMainLoop();
		}
	#endif
}

struct Application::Internal {
	const float performanceFrequency;
	uint64_t currentTime;
	uint64_t previousTime;

	Internal() : performanceFrequency(static_cast<float>(SDL_GetPerformanceFrequency())),
				 currentTime(SDL_GetPerformanceCounter()),
				 previousTime(currentTime) {}

	float TimeStep() {
		previousTime = currentTime;
		currentTime = SDL_GetPerformanceCounter();

		float elapsed{(currentTime - previousTime) * 1000.0f};
		return (elapsed / performanceFrequency) * 0.001f;
	}
};

Application::Application() : internal(RPG::MakeInternalPointer<Internal>()) {}

void Application::StartApplication() {
	#ifdef __EMSCRIPTEN__
		emscripten_set_main_loop_arg((em_arg_callback_func) ::EmscriptenMainLoop, this, 60, 1);
	#else
		while(RunMainLoop()) {}
	#endif
}

bool Application::RunMainLoop() {
	SDL_Event event;

	while (SDL_PollEvent(&event)) {
		switch(event.type) {
			case SDL_WINDOWEVENT:
				if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
					OnWindowResized();
				}
				break;
			case SDL_QUIT:
				return false;
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_ESCAPE) {
					return false;
				}
				break;
			default:
				break;
		}
	}

	//Update this frame
	Update(internal->TimeStep());

	//Render this frame
	Render();

	return true;
}

