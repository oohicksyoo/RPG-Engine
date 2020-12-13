//
// Created by Alex on 12/13/2020.
//

#include "Engine.hpp"
#include "Log.hpp"

using RPG::Engine;

struct Engine::Internal {
	const std::string logTag = "Engine";

	Internal() {}

	void Run() {
		RPG::Log(logTag, "Starting engine up");
	}

	~Internal() {
		RPG::Log(logTag, "Cleaning up Engine");
	}
};

Engine::Engine() : internal(RPG::MakeInternalPointer<Internal>()) {}

void Engine::Run() {
	internal->Run();
}
