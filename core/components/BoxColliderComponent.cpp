//
// Created by Alex on 1/23/2021.
//

#include "BoxColliderComponent.hpp"
#include <memory>

using RPG::BoxColliderComponent;

struct BoxColliderComponent::Internal {
	std::string guid;

	//Properties
	std::shared_ptr<RPG::Property> boxSize;
	std::shared_ptr<RPG::Property> boxIsTrigger;

	Internal(glm::vec3 size, bool isTrigger, std::string guid) : guid(guid),
		boxSize(std::make_unique<RPG::Property>(size, "Size", "glm::vec3")),
		boxIsTrigger(std::make_unique<RPG::Property>(isTrigger, "Trigger", "bool")) {}
};

BoxColliderComponent::BoxColliderComponent(glm::vec3 size, bool isTrigger, std::string guid) : internal(MakeInternalPointer<Internal>(size, isTrigger, guid)) {}

void BoxColliderComponent::Awake() {

}

void BoxColliderComponent::Start() {

}

void BoxColliderComponent::Update(const float &delta) {
}

std::string BoxColliderComponent::Guid() {
	return internal->guid;
}

std::vector<std::shared_ptr<RPG::Property>> BoxColliderComponent::GetProperties() {
	std::vector<std::shared_ptr<RPG::Property>> list = {};
	list.push_back(internal->boxSize);
	list.push_back(internal->boxIsTrigger);

	return list;
}

bool BoxColliderComponent::IsTrigger() {
	return std::any_cast<bool>(internal->boxIsTrigger->GetProperty());
}

glm::vec3 BoxColliderComponent::GetSize() {
	return std::any_cast<glm::vec3>(internal->boxSize->GetProperty());
}

