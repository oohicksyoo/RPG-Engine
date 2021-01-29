//
// Created by Alex on 1/15/2021.
//

#include "InputManager.hpp"
#include "../Time.hpp"
#include "../Log.hpp"

using RPG::InputManager;

InputManager::InputManager() {
	emptyState = RPG::InputState();
	emptyControllerState = RPG::ControllerState();

	emptyControllerState.name = "Disconnected";
	emptyControllerState.isConnected = false;

	for (int i = 0; i < Input::MAX_CONTROLLERS; ++i) {
		emptyState.controllers[i].name = emptyControllerState.name;
	}

	lastState = emptyState;
	currentState = emptyState;
	nextState = emptyState;
}

InputManager::~InputManager() {

}

void InputManager::BeginFrame() {
	//Switch States
	lastState = currentState;
	currentState = nextState;

	//Clear oldState back to default
	{
		//Keyboard
		for (int i = 0; i < Input::MAX_KEYBOARD_KEYS; ++i) {
			if (nextState.keyboard.released[i]) {
				nextState.keyboard.timestamp[i] = 0;
			}

			nextState.keyboard.pressed[i] = false;
			nextState.keyboard.released[i] = false;
		}

		//Mouse
		for (int i = 0; i < Input::MAX_MOUSE_BUTTONS; ++i) {
			if (nextState.mouse.released[i]) {
				nextState.mouse.timestamp[i] = 0;
			}

			nextState.mouse.pressed[i] = false;
			nextState.mouse.released[i] = false;
		}
		nextState.mouse.position = {0,0};
		nextState.mouse.wheel = {0, 0};
		nextState.mouse.delta = {0, 0};

		//Controller
		for (int i = 0; i < Input::MAX_CONTROLLERS; ++i) {
			ControllerState* controller = &(nextState.controllers[i]);

			if (controller->released[i]) {
				controller->buttonTimestamp[i] = 0;
			}

			if (!controller->isConnected) {
				controller->name = "Disconnected";
			}

			for (int j = 0; j < Input::MAX_CONTROLLER_BUTTONS; ++j) {
				controller->pressed[j] = false;
				controller->released[j] = false;
			}
		}
	}
}

RPG::InputState InputManager::GetState() {
	return currentState;
}

RPG::InputState InputManager::GetLastState() {
	return lastState;
}

void InputManager::OnMouseButtonDown(RPG::Input::MouseButton mouseButton) {
	int i = (int)mouseButton;
	if (i >= 0 && i < Input::MAX_MOUSE_BUTTONS) {
		nextState.mouse.down[i] = true;
		nextState.mouse.pressed[i] = true;
		nextState.mouse.timestamp[i] = RPG::Time::milliseconds;
	}
}

void InputManager::OnMouseButtonUp(RPG::Input::MouseButton mouseButton) {
	int i = (int)mouseButton;
	if (i >= 0 && i < Input::MAX_MOUSE_BUTTONS) {
		nextState.mouse.down[i] = false;
		nextState.mouse.released[i] = true;
	}
}

void InputManager::OnMouseWheel(glm::vec2 value) {
	nextState.mouse.wheel = value;
}

void InputManager::OnMousePosition(glm::vec2 value) {
	nextState.mouse.position = value;
}

void InputManager::OnKeyDown(RPG::Input::Key key) {
	int i = (int)key;
	if (i >= 0 && i < Input::MAX_KEYBOARD_KEYS) {
		nextState.keyboard.down[i] = true;
		nextState.keyboard.pressed[i] = true;
		nextState.keyboard.timestamp[i] = RPG::Time::milliseconds;
	}
}

void InputManager::OnKeyUp(RPG::Input::Key key) {
	int i = (int)key;
	if (i >= 0 && i < Input::MAX_KEYBOARD_KEYS) {
		nextState.keyboard.down[i] = false;
		nextState.keyboard.released[i] = true;
	}
}

//TODO: Second time we connect a controller it crashes
void InputManager::OnControllerConnect(int index, std::string name, bool isGamepad, int buttonCount, int axisCount,
									   uint16_t vendor, uint16_t product, uint16_t version) {
	if (index < Input::MAX_CONTROLLERS) {
		RPG::Log("Input", "Connected " + std::to_string(index));
		ControllerState* controller = &(nextState.controllers[index]);
		*controller = emptyControllerState;
		controller->name = name;
		controller->isConnected = true;
		controller->isGamePad = isGamepad;
		controller->buttonCount = buttonCount;
		controller->axisCount = axisCount;
		controller->vendor = vendor;
		controller->product = product;
		controller->version = version;
	}
}

void InputManager::OnControllerDisconnect(int index) {
	if (index < Input::MAX_CONTROLLERS) {
		if (nextState.controllers[index].isConnected) {
			RPG::Log("Input", "Disconnected " + std::to_string(index));
			ControllerState* controller = &(nextState.controllers[index]);
			*controller = emptyControllerState;
			controller->name = "Disconnected";
			controller->isConnected = false;
		}
	}
}

void InputManager::OnControllerButtonDown(int index, int controllerButton) {
	if (index < RPG::Input::MAX_CONTROLLERS &&
			controllerButton < RPG::Input::MAX_CONTROLLER_BUTTONS &&
			nextState.controllers[index].isConnected &&
			controllerButton < nextState.controllers[index].buttonCount) {
		nextState.controllers[index].down[controllerButton] = 1;
		nextState.controllers[index].pressed[controllerButton] = 1;
		nextState.controllers[index].buttonTimestamp[controllerButton] = RPG::Time::milliseconds;
	}
}

void InputManager::OnControllerButtonUp(int index, int controllerButton) {
	if (index < RPG::Input::MAX_CONTROLLERS &&
			controllerButton < RPG::Input::MAX_CONTROLLER_BUTTONS &&
			nextState.controllers[index].isConnected &&
			controllerButton < nextState.controllers[index].buttonCount) {
		nextState.controllers[index].down[controllerButton] = 0;
		nextState.controllers[index].released[controllerButton] = 1;
	}
}

void InputManager::OnControllerAxisMove(int index, int axis, float value) {
	if (index < RPG::Input::MAX_CONTROLLERS &&
		axis < RPG::Input::MAX_CONTROLLER_AXIS &&
		nextState.controllers[index].isConnected &&
			axis < nextState.controllers[index].axisCount) {
		nextState.controllers[index].axis[axis] = value;
		nextState.controllers[index].axisTimestamp[axis] = RPG::Time::milliseconds;
	}
}

bool InputManager::IsKeyDown(RPG::Input::Key key) {
	int i = (int)key;
	return i > 0 && i < Input::MAX_KEYBOARD_KEYS && currentState.keyboard.down[i];
}

bool InputManager::IsKeyPressed(RPG::Input::Key key) {
	int i = (int) key;
	return i > 0 && i < Input::MAX_KEYBOARD_KEYS && currentState.keyboard.pressed[i];
}

bool InputManager::IsKeyReleased(RPG::Input::Key key) {
	int i = (int)key;
	return i > 0 && i < Input::MAX_KEYBOARD_KEYS && currentState.keyboard.released[i];
}

uint64_t InputManager::GetKeyHeldTime(RPG::Input::Key key) {
	int i = (int)key;
	if (i > 0 && i < Input::MAX_KEYBOARD_KEYS) {
		return currentState.keyboard.timestamp[i];
	}
	return 0;
}

bool InputManager::IsMouseButtonDown(RPG::Input::MouseButton mouseButton) {
	int i = (int) mouseButton;
	return i >= 0 && i < Input::MAX_MOUSE_BUTTONS && currentState.mouse.down[i];
}

bool InputManager::IsMouseButtonPressed(RPG::Input::MouseButton mouseButton) {
	int i = (int) mouseButton;
	return i >= 0 && i < Input::MAX_MOUSE_BUTTONS && currentState.mouse.pressed[i];
}

bool InputManager::IsMouseButtonReleased(RPG::Input::MouseButton mouseButton) {
	int i = (int) mouseButton;
	return i >= 0 && i < Input::MAX_MOUSE_BUTTONS && currentState.mouse.released[i];
}

uint64_t InputManager::GetKeyHeldTime(RPG::Input::MouseButton mouseButton) {
	int i = (int)mouseButton;
	if (i > 0 && i < Input::MAX_MOUSE_BUTTONS) {
		return currentState.mouse.timestamp[i];
	}
	return 0;
}

glm::vec2 InputManager::GetMousePosition() {
	return currentState.mouse.position;
}

glm::vec2 InputManager::GetMouseWheel() {
	return currentState.mouse.wheel;
}

bool InputManager::IsControllerButtonDown(int controllerID, RPG::Input::ControllerButton controllerButton) {
	int i = (int)controllerButton;
	bool controllerIDValid = controllerID >= 0 && controllerID < Input::MAX_CONTROLLERS;
	return i >= 0 && i < Input::MAX_CONTROLLER_BUTTONS && controllerIDValid && currentState.controllers[controllerID].isConnected && currentState.controllers[controllerID].down[i];
}

bool InputManager::IsControllerButtonPressed(int controllerID, RPG::Input::ControllerButton controllerButton) {
	int i = (int)controllerButton;
	bool controllerIDValid = controllerID >= 0 && controllerID < Input::MAX_CONTROLLERS;
	return i >= 0 && i < Input::MAX_CONTROLLER_BUTTONS && controllerIDValid && currentState.controllers[controllerID].isConnected && currentState.controllers[controllerID].pressed[i];
}

bool InputManager::IsControllerButtonReleased(int controllerID, RPG::Input::ControllerButton controllerButton) {
	int i = (int)controllerButton;
	bool controllerIDValid = controllerID >= 0 && controllerID < Input::MAX_CONTROLLERS;
	return i >= 0 && i < Input::MAX_CONTROLLER_BUTTONS && controllerIDValid && currentState.controllers[controllerID].isConnected && currentState.controllers[controllerID].released[i];
}

uint64_t InputManager::GetControllerButtonHeldTime(int controllerID, RPG::Input::ControllerButton controllerButton) {
	int i = (int)controllerButton;
	bool controllerIDValid = controllerID >= 0 && controllerID < Input::MAX_CONTROLLERS;
	if (i >= 0 && i < Input::MAX_CONTROLLER_BUTTONS && controllerIDValid && currentState.controllers[controllerID].isConnected) {
		return currentState.controllers[controllerID].buttonTimestamp[i];
	}
	return 0;
}

float InputManager::GetControllerAxis(int controllerID, RPG::Input::ControllerAxis controllerAxis) {
	int i = (int)controllerAxis;
	bool controllerIDValid = controllerID >= 0 && controllerID < Input::MAX_CONTROLLERS;
	if (i >= 0 && i < Input::MAX_CONTROLLER_AXIS && controllerIDValid && currentState.controllers[controllerID].isConnected) {
		return currentState.controllers[controllerID].axis[i];
	}
	return 0;
}

uint64_t InputManager::GetControllerAxisHeldTime(int controllerID, RPG::Input::ControllerAxis controllerAxis) {
	int i = (int)controllerAxis;
	bool controllerIDValid = controllerID >= 0 && controllerID < Input::MAX_CONTROLLERS;
	if (i >= 0 && i < Input::MAX_CONTROLLER_AXIS && controllerIDValid && currentState.controllers[controllerID].isConnected) {
		return currentState.controllers[controllerID].axisTimestamp[i];
	}
	return 0;
}

std::string InputManager::GetNameOf(RPG::Input::Key key) {
	switch(key) {
		#define DEFINE_KEY(name, value) case Input::Key::name: return #name;
			RPG_KEY_DEFINITIONS
		#undef DEFINE_KEY
		default:
			return "Unknown";
	}
}

std::string InputManager::GetNameOf(RPG::Input::MouseButton mouseButton) {
	switch(mouseButton) {
		case RPG::Input::MouseButton::Left:
			return "Left";
		case RPG::Input::MouseButton::Middle:
			return "Middle";
		case RPG::Input::MouseButton::Right:
			return "Right";
		default:
			return "Unknown";
	}
}

std::string InputManager::GetNameOf(RPG::Input::ControllerButton controllerButton) {
	switch(controllerButton) {
		#define DEFINE_BTN(name, value) case Input::ControllerButton::name: return #name;
		RPG_BUTTON_DEFINITIONS
		#undef DEFINE_BTN
		default:
			return "Unknown";
	}
}

std::string InputManager::GetNameOf(RPG::Input::ControllerAxis controllerAxis) {
	switch(controllerAxis) {
		case RPG::Input::ControllerAxis::LeftX:
			return "LeftX";
		case RPG::Input::ControllerAxis::LeftY:
			return "LeftY";
		case RPG::Input::ControllerAxis::RightX:
			return "RightX";
		case RPG::Input::ControllerAxis::RightY:
			return "RightY";
		case RPG::Input::ControllerAxis::LeftTrigger:
			return "Left Trigger";
		case RPG::Input::ControllerAxis::RightTrigger:
			return "Right Trigger";
		default:
			return "Unknown";
	}
}