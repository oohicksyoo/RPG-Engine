//
// Created by Alex on 12/13/2020.
//

#include "Platform.hpp"

RPG::Platform RPG::GetCurrentPlatform() {
	#if defined(__EMSCRIPTEN__)
		return RPG::Platform::emscripten;
	#elif __APPLE__
		#include "TargetConditionals.h"
		#if TARGET_OS_IPHONE
			return RPG::Platform::iOS;
		#else
			return RPG::Platform::mac;
		#endif
	#elif __ANDROID__
		return RPG::Platform::android;
	#elif WIN32
		return RPG::Platform::windows;
	#endif
}
