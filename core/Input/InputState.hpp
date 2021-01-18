//
// Created by Alex on 1/15/2021.
//

#pragma once
#include "Input.hpp"
#include "ControllerState.hpp"
#include "KeyboardState.hpp"
#include "MouseState.hpp"

namespace RPG {
	//Holds the the state for keyboard, mouse & controller
	struct InputState {

		//Holds reference to all possible controllers and not those just connected
		RPG::ControllerState controllers[Input::MAX_CONTROLLERS];
		RPG::KeyboardState keyboard;
		RPG::MouseState mouse;
	};
}


