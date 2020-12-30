//
// Created by Alex on 12/27/2020.
//

#include "Hierarchy.hpp"
#include "GameObject.hpp"
#include <vector>

using RPG::Hierarchy;

struct Hierarchy::Internal {
	std::vector<std::shared_ptr<RPG::GameObject>> hierarchy;

	Internal() {}

	void Add(std::shared_ptr<RPG::GameObject> gameObject) {
		hierarchy.push_back(gameObject);
	}
};

Hierarchy::Hierarchy() : internal(RPG::MakeInternalPointer<Internal>()) {}

void Hierarchy::Add(std::shared_ptr<RPG::GameObject> gameObject, std::shared_ptr<RPG::GameObject> parent) {
	gameObject->SetParent(gameObject, parent);

	if (parent == nullptr) {
		internal->Add(gameObject);
	}
}

void Hierarchy::Remove(std::shared_ptr<RPG::GameObject> gameObject) {
	for (int i = 0; i < internal->hierarchy.size(); ++i) {
		if (internal->hierarchy[i]->GetGuid() == gameObject->GetGuid()) {
			internal->hierarchy.erase(internal->hierarchy.begin() + i);
			return;
		}
	}
}

std::shared_ptr<RPG::GameObject> Hierarchy::GetGameObjectByGuid(std::string guid) {
	//TODO: Finish this functionality
	return internal->hierarchy[0];
}

std::vector<std::shared_ptr<RPG::GameObject>> Hierarchy::GetHierarchy() {
	return internal->hierarchy;
}