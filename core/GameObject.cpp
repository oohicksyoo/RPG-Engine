//
// Created by Alex on 12/27/2020.
//

#include "GameObject.hpp"
#include "Log.hpp"
#include "Components/TransformComponent.hpp"
#include <vector>
#include <string>
#include <memory>

using RPG::GameObject;

struct GameObject::Internal {
	std::string name;
	std::string guid;
	std::vector<std::shared_ptr<RPG::GameObject>> children;
	std::vector<std::shared_ptr<RPG::IComponent>> components;

	Internal(std::string gameObjectName) : name(gameObjectName),
										   guid("1111-1111-1111-1111") {
		children = std::vector<std::shared_ptr<RPG::GameObject>>();
		components.push_back(std::make_unique<RPG::TransformComponent>(RPG::TransformComponent()));
	}

	bool AddComponent(std::shared_ptr<RPG::IComponent> component) {
		std::string name = component->Name();
		RPG::Log("GameObject", "Component: " + name);

		for (auto c : components) {
			if (c->Name() == name) {
				return false;
			}
		}

		components.push_back(component);
		return true;
	}
};

GameObject::GameObject(std::string name) : internal(RPG::MakeInternalPointer<Internal>(name)) {}

std::shared_ptr<RPG::IComponent> GameObject::AddComponent(std::shared_ptr<RPG::IComponent> component) {
	return internal->AddComponent(component) ? component : nullptr;
}

std::string GameObject::GetName() {
	return internal->name;
}

std::string GameObject::GetGuid() {
	return internal->guid;
}

std::vector<std::shared_ptr<RPG::GameObject>> GameObject::GetChildren() {
	return internal->children;
}

bool GameObject::HasChildren() {
	return internal->children.size() > 0;
}