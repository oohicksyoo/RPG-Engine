//
// Created by Alex on 12/27/2020.
//

#include "CameraComponent.hpp"
#include "../Guid.hpp"
#include <memory>

using RPG::CameraComponent;

struct CameraComponent::Internal {
	std::string guid;
	float width;
	float height;

	const glm::mat4 projectionMatrix;
	glm::mat4 viewMatrix;
	glm::vec3 target;
	float pitch = 0;
	float yaw = 0;
	float distance = 10;

	//Properties
	std::shared_ptr<RPG::Property> cameraType;
	//std::shared_ptr<RPG::Property> cameraDistance;

	Internal(float width, float height, std::string guid) :
			     guid(guid),
			     width(width),
			     height(height),
				 cameraType(std::make_unique<RPG::Property>(RPG::CameraType::Perspective, "Camera Type", "RPG::CameraType")),
				 //cameraDistance(std::make_unique<RPG::Property>(10, "Distance", "int")),
				 projectionMatrix(glm::perspective(glm::radians(60.0f), width / height, 0.01f, 100.0f)),
				 target({0, 0, 0}) {
		UpdateViewMatrix();
	}

	glm::vec2 PanSpeed() {
		float x = std::min(width / 1000.0f, 2.4f);
		float xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

		float y = std::min(height / 1000.0f, 2.4f);
		float yFactor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

		return {xFactor, yFactor};
	}

	float ZoomSpeed() {
		//float distance = std::any_cast<float>(cameraDistance);
		float d = distance * 0.2f;
		d = std::max(d, 0.0f);
		float speed = d * d;
		speed = std::min(speed, 100.0f);
		return speed;
	}

	float RotationSpeed() {
		return 0.8f;
	}

	void Pan(glm::vec2 delta) {
		glm::vec2 panSpeed = PanSpeed();
		//float distance = std::any_cast<float>(cameraDistance);
		target += -GetRightDirection() * delta.x * panSpeed.x * distance;
		target += GetUpDirection() * delta.y * panSpeed.y * distance;
	}

	void Rotate(glm::vec2 delta) {
		float yawSign = GetUpDirection().y < 0 ? -1.0f : 1.0f;
		yaw += yawSign * delta.x * RotationSpeed();
		pitch += delta.y * RotationSpeed();
	}

	void Zoom(float delta) {
		//float distance = std::any_cast<float>(cameraDistance);
		distance -= delta * ZoomSpeed();
		if (distance < 1.0f) {
			target += GetForwardDirection();
			distance = 1.0f;
		}
		//cameraDistance->SetProperty(distance);
	}

	void UpdateViewMatrix() {
		glm::vec3 cameraPosition = CalculatePosition();

		glm::quat orientation = GetOrientation();
		viewMatrix = glm::translate(glm::mat4(1.0f), cameraPosition) * glm::toMat4(orientation);
		viewMatrix = glm::inverse(viewMatrix);
	}

	glm::vec3 GetUpDirection() {
		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	glm::vec3 GetRightDirection() {
		return glm::rotate(GetOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
	}

	glm::vec3 GetForwardDirection() {
		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
	}

	glm::vec3 CalculatePosition() {
		//float distance = std::any_cast<float>(cameraDistance);
		return target - GetForwardDirection() * distance;
	}

	glm::quat GetOrientation() {
		return glm::quat(glm::vec3(-pitch, -yaw, 0.0f));
	}
};

CameraComponent::CameraComponent(float width, float height, std::string guid) : internal(MakeInternalPointer<Internal>(width, height, guid)) {}

void CameraComponent::Awake() {

}

void CameraComponent::Start() {

}

void CameraComponent::Update(const float &delta) {
	internal->UpdateViewMatrix();
}

std::string CameraComponent::Guid() {
	return internal->guid;
}

std::vector<std::shared_ptr<RPG::Property>> CameraComponent::GetProperties() {
	std::vector<std::shared_ptr<RPG::Property>> list = {};
	list.push_back(internal->cameraType);
	//list.push_back(internal->cameraDistance);

	return list;
}

glm::mat4 CameraComponent::GetProjectionMatrix() const {
	return internal->projectionMatrix;
}

glm::mat4 CameraComponent::GetViewMatrix(glm::vec3 position) const {
	return internal->viewMatrix;
}

void CameraComponent::Pan(glm::vec2 delta) {
	internal->Pan(delta);
}

void CameraComponent::Rotate(glm::vec2 delta) {
	internal->Rotate(delta);
}

void CameraComponent::Zoom(float delta) {
	internal->Zoom(delta);
}