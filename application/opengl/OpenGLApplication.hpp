//
// Created by Alex on 12/13/2020.
//

#pragma once

#include "../../core/InternalPointer.hpp"
#include "../Application.hpp"

namespace RPG {
	struct OpenGLApplication : public RPG::Application {
		public:
			OpenGLApplication();
			void Render() override;
			void Update(const float& delta) override;
			void OnWindowResized() override;
			void OnGeneralEventData(SDL_Event event) override;
			glm::vec2 GetWindowPosition() override;

		private:
			struct Internal;
			RPG::InternalPointer<Internal> internal;
	};
}


