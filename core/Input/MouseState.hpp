//
// Created by Alex on 1/15/2021.
//

#pragma once
#include "Input.hpp"
#include "../GLMWrapper.hpp"

namespace RPG {
	struct MouseState {
		bool pressed[Input::MAX_MOUSE_BUTTONS];
		bool down[Input::MAX_MOUSE_BUTTONS];
		bool released[Input::MAX_MOUSE_BUTTONS];
		uint64_t timestamp[Input::MAX_MOUSE_BUTTONS];
		glm::vec2 position;
		glm::vec2 wheel;
		glm::vec2 delta;
	};
}
