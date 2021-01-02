//
// Created by Alex on 12/30/2020.
//

#include "LuaScriptComponent.hpp"

using RPG::LuaScriptComponent;

struct LuaScriptComponent::Internal {
	std::shared_ptr<RPG::Property> path;

	Internal(std::string path)  : path(std::make_unique<RPG::Property>(path, "Path", "std::string")) {}
};

LuaScriptComponent::LuaScriptComponent(std::string path) : internal(MakeInternalPointer<Internal>(path)) {}

void LuaScriptComponent::Awake() {

}

void LuaScriptComponent::Start() {

}

void LuaScriptComponent::Update() {

}

std::vector<std::shared_ptr<RPG::Property>> LuaScriptComponent::GetProperties() {
	std::vector<std::shared_ptr<RPG::Property>> list = {};
	list.push_back(internal->path);

	return list;
}