//
// Created by Alex on 1/15/2021.
//

#pragma once
#include "../Singleton.hpp"
#include "InputState.hpp"
#include "ControllerState.hpp"
#include "Key.hpp"
#include "ControllerButton.hpp"
#include "MouseButton.hpp"
#include "ControllerAxis.hpp"
#include <memory>

namespace RPG {
	struct InputManager : public Singleton<InputManager> {
		friend struct Singleton<InputManager>;

		public:
			InputManager();
			~InputManager();

			//Rendering to cycle states
			void BeginFrame();

			//Get States
			RPG::InputState GetState();
			RPG::InputState GetLastState();

			//Functionality to Change State
			void OnMouseButtonDown(RPG::Input::MouseButton mouseButton);
			void OnMouseButtonUp(RPG::Input::MouseButton mouseButton);
			void OnMouseWheel(glm::vec2 value);
			void OnMousePosition(glm::vec2 value);
			void OnKeyDown(RPG::Input::Key key);
			void OnKeyUp(RPG::Input::Key key);
			void OnControllerConnect(int index, std::string name, bool isGamepad, int buttonCount, int axisCount, uint16_t vendor, uint16_t product, uint16_t version);
			void OnControllerDisconnect(int index);
			void OnControllerButtonDown(int index, int controllerButton);
			void OnControllerButtonUp(int index, int controllerButton);
			void OnControllerAxisMove(int index, int axis, float value);

			//Get Functionality to Avoid Using States
			//Keyboard
			bool IsKeyPressed(RPG::Input::Key key);
			bool IsKeyDown(RPG::Input::Key key);
			bool IsKeyReleased(RPG::Input::Key key);
			uint64_t GetKeyHeldTime(RPG::Input::Key key);

			//Mouse
			bool IsMouseButtonPressed(RPG::Input::MouseButton mouseButton);
			bool IsMouseButtonDown(RPG::Input::MouseButton mouseButton);
			bool IsMouseButtonReleased(RPG::Input::MouseButton mouseButton);
			uint64_t GetKeyHeldTime(RPG::Input::MouseButton mouseButton);
			glm::vec2 GetMousePosition();
			glm::vec2 GetMouseWheel();

			//Get Name Of
			std::string GetNameOf(RPG::Input::Key key);
			std::string GetNameOf(RPG::Input::MouseButton mouseButton);
			std::string GetNameOf(RPG::Input::ControllerButton controllerButton);
			std::string GetNameOf(RPG::Input::ControllerAxis controllerAxis);

		private:
			RPG::InputState lastState;
			RPG::InputState currentState;
			RPG::InputState nextState;
			RPG::InputState emptyState;
			RPG::ControllerState emptyControllerState;
	};
}


