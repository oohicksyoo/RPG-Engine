//
// Created by Alex on 12/13/2020.
//

#include "Engine.hpp"
#include "Log.hpp"
#include "../application/Application.hpp"
#include "../application/opengl/OpenGLApplication.hpp"
#ifndef __EMSCRIPTEN__
	#include "../application/vulkan/VulkanApplication.hpp"
	#include "../application/vulkan/VulkanCommon.hpp"
#endif
#include <stdexcept>
#include <SDL_image.h>
#include "Serializer.hpp"

using RPG::Engine;

struct Engine::Internal {
	const std::string logTag = "Engine";

	Internal() {}

	void Run() {
		RPG::Log(logTag, "Starting engine up");
		SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER);
		RPG::Log(logTag, "SDL2 initialized successfully");

		if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
			throw std::runtime_error("[" + logTag + "] Could not initialize SDL2_Image");
		}
		RPG::Log(logTag, "SDL2_image initialized successfully with PNG support");

		RPG::Serializer::GetInstance().LoadDefaultPropertyTypes();

		ResolveApplication()->StartApplication();
	}

	//Determine if the application can run Vulkan and if not fall back to a version of OpenGL
	std::unique_ptr<RPG::Application> ResolveApplication() {
		#ifdef FORCE_OPENGL
			RPG::Log(logTag, "Forcing OpenGL");
		#endif

		#if !defined(__EMSCRIPTEN__) && !defined(FORCE_OPENGL)
			if (RPG::Vulkan::IsVulkanAvailable()) {
				try {
					RPG::Log(logTag, "Creating Vulkan application ...");
					return std::make_unique<RPG::VulkanApplication>();
				} catch (const std::exception& error) {
					RPG::Log(logTag, "Vulkan application failed to initialize", error);
				}
			}
		#endif

		try {
			RPG::Log(logTag, "Creating OpenGL application ...");
			return std::make_unique<RPG::OpenGLApplication>();
		} catch (const std::exception& error) {
			RPG::Log(logTag, "OpenGL application failed to initialize", error);
		}

		throw std::runtime_error("[" + logTag + "] No applications can run in the current environment");
	}

	~Internal() {
		RPG::Log(logTag, "Cleaning up Engine");
		SDL_Quit();
	}
};

Engine::Engine() : internal(RPG::MakeInternalPointer<Internal>()) {}

void Engine::Run() {
	internal->Run();
}
