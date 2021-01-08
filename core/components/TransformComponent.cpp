//
// Created by Alex on 12/27/2020.
//

#include "TransformComponent.hpp"
#include "../Guid.hpp"

using RPG::TransformComponent;

struct TransformComponent::Internal {
	std::string guid;
	std::shared_ptr<RPG::Property> position;
	std::shared_ptr<RPG::Property> rotation;
	std::shared_ptr<RPG::Property> scale;
	const glm::mat4 identity;
	RPG::Action<>::Func<std::shared_ptr<RPG::TransformComponent>> getParentFunc;

	Internal(std::string guid) : guid(guid),
			     position(std::make_unique<RPG::Property>(glm::vec3{0, 0, 0}, "Position", "glm::vec3")),
				 rotation(std::make_unique<RPG::Property>(glm::vec3{0, 0, 0}, "Rotation", "glm::vec3")),
				 scale(std::make_unique<RPG::Property>(glm::vec3{1, 1, 1}, "Scale", "glm::vec3")),
				 identity(glm::mat4{1.0f}) {}
};

TransformComponent::TransformComponent(std::string guid) : internal(MakeInternalPointer<Internal>(guid)) {}

void TransformComponent::Awake() {

}

void TransformComponent::Start() {

}

void TransformComponent::Update(const float &delta) {

}

std::string TransformComponent::Guid() {
	return internal->guid;
}

glm::mat4 TransformComponent::GetTransformMatrix() {
	glm::vec3 rotation = std::any_cast<glm::vec3>(internal->rotation->GetProperty());
	glm::fquat rot{rotation};

	glm::mat4 modelMatrix = internal->identity;
	modelMatrix = glm::translate(modelMatrix, std::any_cast<glm::vec3>(internal->position->GetProperty()));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.x), glm::vec3{1.0f, 0.0f, 0.0f});
	modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.y), glm::vec3{0.0f, 1.0f, 0.0f});
	modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.z), glm::vec3{0.0f, 0.0f, 1.0f});
	modelMatrix = glm::scale(modelMatrix, std::any_cast<glm::vec3>(internal->scale->GetProperty()));

	//Using parent we need to multiply this by the parents GetTransformMatrix as well
	auto parentTransform = internal->getParentFunc();
	if (parentTransform != nullptr) {
		modelMatrix = parentTransform->GetTransformMatrix() * modelMatrix;
	}

	return modelMatrix;
}

std::vector<std::shared_ptr<RPG::Property>> TransformComponent::GetProperties() {
	std::vector<std::shared_ptr<RPG::Property>> list = {};
	list.push_back(internal->position);
	list.push_back(internal->rotation);
	list.push_back(internal->scale);

	return list;
}

void TransformComponent::SetGetParent(RPG::Action<>::Func<std::shared_ptr<RPG::TransformComponent>> getParentFunc) {
	internal->getParentFunc = getParentFunc;
}

void TransformComponent::SetPosition(glm::vec3 position) {
	internal->position->SetProperty(position);
}

glm::vec3 TransformComponent::GetPosition() {
	return std::any_cast<glm::vec3>(internal->position->GetProperty());
}

void TransformComponent::SetRotation(glm::vec3 rotation) {
	internal->rotation->SetProperty(rotation);
}

glm::vec3 TransformComponent::GetRotation() {
	return std::any_cast<glm::vec3>(internal->rotation->GetProperty());
}

void TransformComponent::SetScale(glm::vec3 scale) {
	internal->scale->SetProperty(scale);
}

glm::vec3 TransformComponent::GetScale() {
	return std::any_cast<glm::vec3>(internal->scale->GetProperty());
}