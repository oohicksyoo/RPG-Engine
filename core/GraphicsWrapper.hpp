//
// Created by Alex on 12/13/2020.
//

#pragma once

#if defined(__EMSCRIPTEN__)
	#include<GLES2/gl2.h>
	#define USING_GLES
#elif __APPLE__
	#define GL_SILENCE_DEPRECATION
	#include <vulkan/vulkan.hpp>
	#include "TargetConditionals.h"
	#if TARGET_OS_IPHONE
		#include <OpenGLES/ES2/gl.h>
		#define USING_GLES
	#else
		#include <OpenGL/gl3.h>
	#endif
#elif __ANDROID__
	#include <vulkan_wrapper.h>
	#include <vulkan/vulkan.hpp>
	#include <GLES2/gl2.h>
	#define USING_GLES
#elif WIN32
	#define GLEW_STATIC
	#include <GL/glew.h>
	#include <vulkan/vulkan.hpp>
#endif
