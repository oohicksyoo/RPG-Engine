//
// Created by Alex on 12/27/2020.
//

#include "GameObject.hpp"
#include "Log.hpp"
#include "Guid.hpp"
#include "components/PhysicsComponent.hpp"
#include "PhysicsSystem.hpp"
#include <vector>
#include <string>
#include <memory>

using RPG::GameObject;

struct GameObject::Internal {
	std::string name;
	std::string guid;
	std::shared_ptr<RPG::TransformComponent> transform;
	std::vector<std::shared_ptr<RPG::GameObject>> children;
	std::vector<std::shared_ptr<RPG::IComponent>> components;
	std::shared_ptr<RPG::GameObject> parent;

	Internal(std::string gameObjectName, std::string guid) : name(gameObjectName),
										   guid(guid),
										   transform(std::make_unique<RPG::TransformComponent>(RPG::TransformComponent())) {
		transform->SetGetParent([this]() -> std::shared_ptr<RPG::TransformComponent> {
			//Do we have parent
			if (parent != nullptr) {
				return parent->GetTransform();
			}
			return nullptr;
		});
		AddComponent(transform);
	}

	~Internal() {
		RPG::Log("GameObject", "Cleanup - (" + name + "|" + guid + ") is being removed from memory");
	}

	bool AddComponent(std::shared_ptr<RPG::IComponent> component) {
		std::string name = component->Name();
		//RPG::Log("GameObject", "Component: " + name + "\nAllow Multiples? " + (component->AllowMultiple() ? "True" : "False"));

		if (!component->AllowMultiple()) {
			for (auto c : components) {
				if (c->Name() == name) {
					return false;
				}
			}
		}

		components.push_back(component);
		return true;
	}

	void RemoveComponent(std::string guid) {
		int index = -1;
		for (int i = 0; i < components.size(); ++i) {
			if (components[i]->Guid() == guid) {
				index = i;
			}
		}

		if (index >= 0) {
			components.erase(components.begin() + index);
		}
	}
};

GameObject::GameObject(std::string name, std::string guid) : internal(RPG::MakeInternalPointer<Internal>(name, guid)) {}

void GameObject::Awake() {
	for (auto child : internal->children) {
		child->Awake();
	}

	for (auto component : internal->components) {
		component->Awake();
	}


	auto pc = GetComponent<std::shared_ptr<RPG::PhysicsComponent>, RPG::PhysicsComponent>("PhysicsComponent");
	if (pc != nullptr) {
		RPG::PhysicsSystem::GetInstance().RegisterPhysicsComponent(pc);
	}
}

void GameObject::Start() {
	for (auto child : internal->children) {
		child->Start();
	}

	for (auto component : internal->components) {
		component->Start();
	}
}

void GameObject::Update(const float &delta) {
	for (auto child : internal->children) {
		child->Update(delta);
	}

	for (auto component : internal->components) {
		component->Update(delta);
	}
}

std::shared_ptr<RPG::IComponent> GameObject::AddComponent(std::shared_ptr<RPG::IComponent> component) {
	return internal->AddComponent(component) ? component : nullptr;
}

void GameObject::RemoveComponent(std::string guid) {
	return internal->RemoveComponent(guid);
}

std::vector<std::shared_ptr<RPG::IComponent>> GameObject::GetComponents() {
	return internal->components;
}

std::string GameObject::GetName() {
	return internal->name;
}

void GameObject::SetName(std::string name) {
	internal->name = name;
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

bool GameObject::AddChild(std::shared_ptr<RPG::GameObject> gameObject) {
	for (auto child : internal->children) {
		if (child->GetGuid() == gameObject->GetGuid()) {
			return false;
		}
	}

	internal->children.push_back(gameObject);
	RPG::Log("GameObject", "Added GameObject (" + gameObject->GetName() + "|" + gameObject->GetGuid() + ") as a child of (" +  internal->name + "|" + internal->guid + ")");
	return true;
}

bool GameObject::RemoveChild(std::shared_ptr<RPG::GameObject> gameObject) {
	for (int i = 0; i < internal->children.size(); ++i) {
		if (internal->children[i]->GetGuid() == gameObject->GetGuid()) {
			internal->children.erase(internal->children.begin() + i);
			return true;
		}
	}

	return false;
}

void GameObject::SetParent(std::shared_ptr<RPG::GameObject> gameObject, std::shared_ptr<RPG::GameObject> parent) {
	if (internal->parent != nullptr) {
		//Remove myself as a child from this gameobject
		bool value = internal->parent->RemoveChild(gameObject);
		if (!value) {
			RPG::Log("GameObject" , "Failed to remove child (" + internal->name + "|" + internal->guid + ") from (" + internal->parent->GetName() + ")");
		}
	}

	internal->parent = parent; //This could be nullptr if the gameobject was directly added to the top of hierarchy
	if (internal->parent != nullptr) {
		bool value = internal->parent->AddChild(gameObject);
		if (!value) {
			RPG::Log("GameObject", "Failed to add child (" + internal->name + "|" + internal->guid + ") from (" +
								   internal->parent->GetName() + ")");
		}
	}
}

bool GameObject::HasParent() {
	return internal->parent != nullptr;
}

std::shared_ptr<RPG::TransformComponent> GameObject::GetTransform() {
	return internal->transform;
}

std::shared_ptr<RPG::GameObject> GameObject::GetParent() {
	return internal->parent;
}

std::vector<std::shared_ptr<RPG::IComponent>> GameObject::GetLuaScripts() {
	std::vector<std::shared_ptr<RPG::IComponent>> list;

	for (auto c : internal->components) {
		if (c->Name() == "LuaScriptComponent") {
			list.push_back(c);
		}
	}

	return list;
}