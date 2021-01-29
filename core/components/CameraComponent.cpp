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
	std::shared_ptr<RPG::TransformComponent> transform;

	//Properties
	std::shared_ptr<RPG::Property> cameraType;
	std::shared_ptr<RPG::Property> cameraDistance;
	std::shared_ptr<RPG::Property> isMainCamera;
	std::shared_ptr<RPG::Property> pitch;
	std::shared_ptr<RPG::Property> yaw;

	Internal(float width, float height, std::shared_ptr<RPG::TransformComponent> transformComponent, std::string guid) :
			     guid(guid),
			     width(width),
			     height(height),
				 transform(transformComponent),
				 cameraType(std::make_unique<RPG::Property>(RPG::CameraType::Perspective, "Camera Type", "RPG::CameraType")),
				 cameraDistance(std::make_unique<RPG::Property>(10.0f, "Distance", "float")),
				 isMainCamera(std::make_unique<RPG::Property>(false, "Is Main Camera", "bool")),
				 pitch(std::make_unique<RPG::Property>(0.0f, "Pitch (Vertical)", "float")),
				 yaw(std::make_unique<RPG::Property>(0.0f, "Yaw (Horizontal)", "float")),
				 projectionMatrix(glm::perspective(glm::radians(60.0f), width / height, 0.01f, 1000.0f)) {
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
		float distance = std::any_cast<float>(cameraDistance->GetProperty());
		float d = distance * 0.2f;
		d = std::max(d, 0.0f);
		float speed = d * d;
		speed = std::min(speed, 100.0f);
		return speed;
	}

	float RotationSpeed() {
		return 50.0f;
	}

	void Pan(glm::vec2 delta) {
		glm::vec2 panSpeed = PanSpeed();
		float distance = std::any_cast<float>(cameraDistance->GetProperty());
		glm::vec3 target = transform->GetWorldPosition();
		target += -GetRightDirection() * delta.x * panSpeed.x * distance;
		target += GetUpDirection() * delta.y * panSpeed.y * distance;
		transform->SetWorldPosition(target);
	}

	void Rotate(glm::vec2 delta) {
		float y = std::any_cast<float>(yaw->GetProperty());
		float p = std::any_cast<float>(pitch->GetProperty());
		float yawSign = GetUpDirection().y < 0 ? -1.0f : 1.0f;
		y += yawSign * delta.x * RotationSpeed();
		p += delta.y * RotationSpeed();

		yaw->SetProperty(y);
		pitch->SetProperty(p);
	}

	void Zoom(float delta) {
		float distance = std::any_cast<float>(cameraDistance->GetProperty());
		distance -= delta * ZoomSpeed();
		if (distance < 1.0f) {
			glm::vec3 target = transform->GetWorldPosition();
			target += GetForwardDirection();
			transform->SetWorldPosition(target);
			distance = 1.0f;
		}
		cameraDistance->SetProperty(distance);
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
		float distance = std::any_cast<float>(cameraDistance->GetProperty());
		glm::vec3 target = transform->GetWorldPosition();
		return target - GetForwardDirection() * distance;
	}

	glm::quat GetOrientation() {
		float y = std::any_cast<float>(yaw->GetProperty());
		float p = std::any_cast<float>(pitch->GetProperty());

		y *= 3.14159f / 180;
		p *= 3.14159f / 180;

		return glm::quat(glm::vec3(-p, -y, 0.0f));
	}
};

CameraComponent::CameraComponent(float width, float height, std::shared_ptr<RPG::TransformComponent> transformComponent, std::string guid) : internal(MakeInternalPointer<Internal>(width, height, transformComponent, guid)) {}

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
	list.push_back(internal->cameraDistance);
	list.push_back(internal->isMainCamera);
	list.push_back(internal->yaw);
	list.push_back(internal->pitch);

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

void CameraComponent::SetDistance(float value) {
	value = (value < 0.1f) ? 0.1f : value;
	internal->cameraDistance->SetProperty(value);
}

void CameraComponent::SetIsMainCamera(bool value) {
	internal->isMainCamera->SetProperty(value);
}

void CameraComponent::SetYaw(float value) {
	internal->yaw->SetProperty(value);
}

void CameraComponent::SetPitch(float value) {
	internal->pitch->SetProperty(value);
}