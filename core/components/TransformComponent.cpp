//
// Created by Alex on 12/27/2020.
//

#include "TransformComponent.hpp"
#include "../GLMWrapper.hpp"

using RPG::TransformComponent;

struct TransformComponent::Internal {
	std::shared_ptr<RPG::Property> position;
	std::shared_ptr<RPG::Property> rotation;
	std::shared_ptr<RPG::Property> scale;

	Internal() : position(std::make_unique<RPG::Property>(glm::vec3{0, 0, 0}, "Position", "glm::vec3")),
				 rotation(std::make_unique<RPG::Property>(glm::vec3{0, 0, 0}, "Rotation", "glm::vec3")),
				 scale(std::make_unique<RPG::Property>(glm::vec3{1, 1, 1}, "Scale", "glm::vec3")) {}
};

TransformComponent::TransformComponent() : internal(MakeInternalPointer<Internal>()) {}

void TransformComponent::Awake() {

}

void TransformComponent::Start() {

}

void TransformComponent::Update() {

}

std::vector<std::shared_ptr<RPG::Property>> TransformComponent::GetProperties() {
	std::vector<std::shared_ptr<RPG::Property>> list = {};
	list.push_back(internal->position);
	list.push_back(internal->rotation);
	list.push_back(internal->scale);

	return list;
}