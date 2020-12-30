//
// Created by Alex on 12/27/2020.
//

#include "TransformComponent.hpp"
#include "../GLMWrapper.hpp"

using RPG::TransformComponent;

struct TransformComponent::Internal {
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;

	Internal() : position(glm::vec3{0, 0, 0}),
				 rotation(glm::vec3{0, 0, 0}),
				 scale(glm::vec3{1, 1, 1}) {}
};

TransformComponent::TransformComponent() : internal(MakeInternalPointer<Internal>()) {}

void TransformComponent::Awake() {

}

void TransformComponent::Start() {

}

void TransformComponent::Update() {

}