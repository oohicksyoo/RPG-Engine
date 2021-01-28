//
// Created by Alex on 12/13/2020.
//

#ifdef __EMSCRIPTEN__
	#include <emscripten.h>
#endif

#include "Application.hpp"
#include "../core/Time.hpp"
#include "../core/input/InputManager.hpp"
#include "../core/Log.hpp"
#include "../core/Singleton.hpp"
#include "../core/PhysicsSystem.hpp"

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
	SDL_Joystick* joysticks[RPG::Input::MAX_CONTROLLERS];
	SDL_GameController* gamepads[RPG::Input::MAX_CONTROLLERS];

	Internal() : performanceFrequency(static_cast<float>(SDL_GetPerformanceFrequency())),
				 currentTime(SDL_GetPerformanceCounter()),
				 previousTime(currentTime) {}

	float TimeStep() {
		previousTime = currentTime;
		currentTime = SDL_GetPerformanceCounter();
		Time::milliseconds = currentTime;

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
	RPG::Input::MouseButton mouseButton;
	Sint32 index;

	//Update the Mouse Data
	{
		int x, y;
		glm::vec2 windowPosition = GetWindowPosition();
		SDL_GetGlobalMouseState(&x, &y);
		InputManager::GetInstance().OnMousePosition(glm::vec2 {x, y} - windowPosition);
	}

	while (SDL_PollEvent(&event)) {
		OnGeneralEventData(event);
		switch(event.type) {
			case SDL_WINDOWEVENT:
				if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
					OnWindowResized();
				}
				break;
			case SDL_QUIT:
				return false;
			case SDL_MOUSEBUTTONDOWN:
				mouseButton = RPG::Input::MouseButton::None;
				if (event.button.button == SDL_BUTTON_LEFT) {
					mouseButton = RPG::Input::MouseButton::Left;
				} else if (event.button.button == SDL_BUTTON_RIGHT) {
					mouseButton = RPG::Input::MouseButton::Right;
				} else if (event.button.button == SDL_BUTTON_MIDDLE) {
					mouseButton = RPG::Input::MouseButton::Middle;
				}
				InputManager::GetInstance().OnMouseButtonDown(mouseButton);
				break;
			case SDL_MOUSEBUTTONUP:
				mouseButton = RPG::Input::MouseButton::None;
				if (event.button.button == SDL_BUTTON_LEFT) {
					mouseButton = RPG::Input::MouseButton::Left;
				} else if (event.button.button == SDL_BUTTON_RIGHT) {
					mouseButton = RPG::Input::MouseButton::Right;
				} else if (event.button.button == SDL_BUTTON_MIDDLE) {
					mouseButton = RPG::Input::MouseButton::Middle;
				}
				InputManager::GetInstance().OnMouseButtonUp(mouseButton);
				break;
			case SDL_MOUSEWHEEL:
				InputManager::GetInstance().OnMouseWheel({event.wheel.x, event.wheel.y});
				break;
			case SDL_KEYDOWN:
				if (event.key.repeat == 0) {
					InputManager::GetInstance().OnKeyDown((RPG::Input::Key)event.key.keysym.scancode);
				}
				break;
			case SDL_KEYUP:
				if (event.key.repeat == 0) {
					InputManager::GetInstance().OnKeyUp((RPG::Input::Key)event.key.keysym.scancode);
				}
				break;
			case SDL_JOYDEVICEADDED:
				 index = event.jdevice.which;
				 if (SDL_IsGameController(index) == SDL_FALSE) {
				 	RPG::Log("Input", "Joystick Connection event");
				 	SDL_Joystick* ptr = internal->joysticks[index] = SDL_JoystickOpen(index);
				 	std::string name = SDL_JoystickName(ptr);
				 	int buttonCount = SDL_JoystickNumButtons(ptr);
				 	int axisCount = SDL_JoystickNumAxes(ptr);
				 	uint16_t vendor = SDL_JoystickGetVendor(ptr);
				 	uint16_t product = SDL_JoystickGetProduct(ptr);
				 	uint16_t version = SDL_JoystickGetProductVersion(ptr);

				 	InputManager::GetInstance().OnControllerConnect(index, name, false, buttonCount, axisCount, vendor, product, version);
				 }
				break;
			/*case SDL_JOYDEVICEREMOVED:
				index = event.jdevice.which;
				if (SDL_IsGameController(index) == SDL_FALSE) {
					RPG::Log("Input", "Joystick Disconnection event");
					InputManager::GetInstance().OnControllerDisconnect(index);
					SDL_JoystickClose(internal->joysticks[index]);
				}
				break;
			case SDL_JOYBUTTONDOWN:
				index = event.jdevice.which;
				if (SDL_IsGameController(index) == SDL_FALSE) {
					InputManager::GetInstance().OnControllerButtonDown(index, event.jbutton.button);
				}
				break;
			case SDL_JOYBUTTONUP:
				index = event.jdevice.which;
				if (SDL_IsGameController(index) == SDL_FALSE) {
					InputManager::GetInstance().OnControllerButtonUp(index, event.jbutton.button);
				}
				break;
			case SDL_JOYAXISMOTION:
				index = event.jdevice.which;
				if (SDL_IsGameController(index) == SDL_FALSE) {
					float value = (event.jaxis.value >= 0) ? event.jaxis.value / 32767.0f : event.jaxis.value / 32768.0f;
					InputManager::GetInstance().OnControllerAxisMove(index, event.jaxis.axis, value);
				}
				break;*/
			case SDL_CONTROLLERDEVICEADDED:
				{
					RPG::Log("Input", "Controller Connection event");
					index = event.cdevice.which;
					SDL_GameController *ptr = internal->gamepads[index] = SDL_GameControllerOpen(index);
					std::string name = SDL_GameControllerName(ptr);
					uint16_t vendor = SDL_GameControllerGetVendor(ptr);
					uint16_t product = SDL_GameControllerGetProduct(ptr);
					uint16_t version = SDL_GameControllerGetProductVersion(ptr);

					InputManager::GetInstance().OnControllerConnect(index, name, true, 15, 6, vendor, product, version);
				}
				break;
			case SDL_CONTROLLERDEVICEREMOVED:
				{
					RPG::Log("Input", "Controller Disconnection event");
					index = event.cdevice.which;
					InputManager::GetInstance().OnControllerDisconnect(index);
					SDL_GameControllerClose(internal->gamepads[index]);
				}
				break;
			case SDL_CONTROLLERBUTTONDOWN:
				{
					index = event.cbutton.which;
					int button = (int) RPG::Input::ControllerButton::None;
					if (event.cbutton.button >= 0 && event.cbutton.button < 15) {
						button = event.cbutton.button;
					}
					InputManager::GetInstance().OnControllerButtonDown(index, button);
				}
				break;
			case SDL_CONTROLLERBUTTONUP:
				{
					index = event.cbutton.which;
					int button = (int) RPG::Input::ControllerButton::None;
					if (event.cbutton.button >= 0 && event.cbutton.button < 15) {
						button = event.cbutton.button;
					}
					InputManager::GetInstance().OnControllerButtonUp(index, button);
				}
				break;
			case SDL_CONTROLLERAXISMOTION:
				{
					index = event.caxis.which;
					int axis = (int) RPG::Input::ControllerAxis::None;
					if (event.caxis.axis >= 0 && event.caxis.axis < 6) {
						axis = event.caxis.axis;
					}
					float value = (event.caxis.value >= 0) ? event.caxis.value / 32767.0f : event.caxis.value / 32768.0f;
					InputManager::GetInstance().OnControllerAxisMove(index, axis, value);
				}
				break;
			default:
				break;
		}
	}

	//Update the Input States
	InputManager::GetInstance().BeginFrame();

	//Update Physics
	RPG::PhysicsSystem::GetInstance().Update(internal->TimeStep());

	//Update this frame
	Update(internal->TimeStep());

	//Render this frame
	Render();

	return true;
}

