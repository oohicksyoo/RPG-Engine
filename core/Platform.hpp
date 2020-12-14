//
// Created by Alex on 12/13/2020.
//

#pragma once

namespace RPG {
	enum class Platform {
			mac,
			iOS,
			android,
			emscripten,
			windows
	};

	Platform GetCurrentPlatform();
}


