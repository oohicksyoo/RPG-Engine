//
// Created by Alex on 1/16/2021.
//

#pragma once

#define RPG_BUTTON_DEFINITIONS \
	DEFINE_BTN(None, -1) \
	DEFINE_BTN(A, 0) \
	DEFINE_BTN(B, 1) \
	DEFINE_BTN(X, 2) \
	DEFINE_BTN(Y, 3) \
	DEFINE_BTN(Back, 4) \
	DEFINE_BTN(Select, 5) \
	DEFINE_BTN(Start, 6) \
	DEFINE_BTN(LeftStick, 7) \
	DEFINE_BTN(RightStick, 8) \
	DEFINE_BTN(LeftShoulder, 9) \
	DEFINE_BTN(RightShoulder, 10) \
	DEFINE_BTN(Up, 11) \
	DEFINE_BTN(Down, 12) \
	DEFINE_BTN(Left, 13) \
	DEFINE_BTN(Right, 14)

namespace RPG {
	namespace Input {
		enum class ControllerButton {
				#define DEFINE_BTN(name, value) name = value,
				RPG_BUTTON_DEFINITIONS
				#undef DEFINE_BTN
		};
	}
}

