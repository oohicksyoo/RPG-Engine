//
// Created by Alex on 1/15/2021.
//

#pragma once
#include "Input.hpp"
#include <string>

namespace RPG {
	struct ControllerState {
		std::string name;
		bool isConnected;
		bool isGamePad;
		int buttonCount;
		int axisCount;
		bool pressed[Input::MAX_CONTROLLER_BUTTONS];
		bool down[Input::MAX_CONTROLLER_BUTTONS];
		bool released[Input::MAX_CONTROLLER_BUTTONS];
		float axis[Input::MAX_CONTROLLER_AXIS];
		uint64_t buttonTimestamp[Input::MAX_CONTROLLER_BUTTONS];
		uint64_t axisTimestamp[Input::MAX_CONTROLLER_AXIS];
		uint16_t vendor; //The USB Vendor ID
		uint16_t product; //The USB Product ID
		uint16_t version; //The Product version
	};
}
