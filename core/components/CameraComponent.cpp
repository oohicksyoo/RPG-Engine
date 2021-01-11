//
// Created by Alex on 12/27/2020.
//

#include "CameraComponent.hpp"
#include "../Guid.hpp"

using RPG::CameraComponent;

struct CameraComponent::Internal {
	std::string guid;
	std::shared_ptr<RPG::Property> cameraType;
	const glm::mat4 projectionMatrix;
	const glm::vec3 up;
	glm::vec3 target;

	Internal(float width, float height, std::string guid) :
			     guid(guid),
				 cameraType(std::make_unique<RPG::Property>(RPG::CameraType::Perspective, "Camera Type", "RPG::CameraType")),
				 projectionMatrix(glm::perspective(glm::radians(60.0f), width / height, 0.01f, 100.0f)),
				 up(glm::vec3{0.0f, 1.0f, 0.0f}),
				 //Target = position - direction
				 target({0, 0, -1}) {}
};

CameraComponent::CameraComponent(float width, float height, std::string guid) : internal(MakeInternalPointer<Internal>(width, height, guid)) {}

void CameraComponent::Awake() {

}

void CameraComponent::Start() {

}

void CameraComponent::Update(const float &delta) {

}

std::string CameraComponent::Guid() {
	return internal->guid;
}

std::vector<std::shared_ptr<RPG::Property>> CameraComponent::GetProperties() {
	std::vector<std::shared_ptr<RPG::Property>> list = {};
	list.push_back(internal->cameraType);

	return list;
}

glm::mat4 CameraComponent::GetProjectionMatrix() const {
	return internal->projectionMatrix;
}

glm::mat4 CameraComponent::GetViewMatrix(glm::vec3 position) const {
	return glm::lookAt(position, internal->target, internal->up);
}