//
// Created by Alex on 12/13/2020.
//

#pragma once

#include "../core/InternalPointer.hpp"
#include "../core/GLMWrapper.hpp"
#include "../core/SDLWrapper.hpp"

namespace RPG {
	struct Application {
		public:
			Application();
			virtual ~Application() = default;
			void StartApplication();
			bool RunMainLoop();
			virtual void Render() = 0;
			virtual void Update(const float& delta) = 0;
			virtual void OnWindowResized() = 0;
			virtual void OnGeneralEventData(SDL_Event event) = 0;

		private:
			struct Internal;
			RPG::InternalPointer<Internal> internal;
	};
}


