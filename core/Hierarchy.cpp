//
// Created by Alex on 12/27/2020.
//

#include "Hierarchy.hpp"
#include "GameObject.hpp"
#include <vector>

using RPG::Hierarchy;

struct Hierarchy::Internal {
	std::vector<RPG::GameObject> hierarchy;

	Internal() {}
};

Hierarchy::Hierarchy() : internal(RPG::MakeInternalPointer<Internal>()) {}

void Hierarchy::Add(RPG::GameObject gameObject, RPG::TransformComponent parent) {

}

RPG::GameObject Hierarchy::GetGameObjectByGuid(std::string guid) {

}