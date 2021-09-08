//
// Created by Alex on 1/16/2021.
//

#pragma once

namespace RPG {
	struct Time {
		inline static uint64_t milliseconds;
		inline static float Seconds() {
		    return milliseconds / 10000000.0f;
		};
	};
}
