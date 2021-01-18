//
// Created by Alex on 1/15/2021.
//

#pragma once
#include "Input.hpp"

namespace RPG {
	struct KeyboardState {
		bool pressed[Input::MAX_KEYBOARD_KEYS];
		bool down[Input::MAX_KEYBOARD_KEYS];
		bool released[Input::MAX_KEYBOARD_KEYS];
		uint64_t timestamp[Input::MAX_KEYBOARD_KEYS];
	};
}