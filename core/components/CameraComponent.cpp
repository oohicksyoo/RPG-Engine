//
// Created by Alex on 12/27/2020.
//

#include "CameraComponent.hpp"

using RPG::CameraComponent;

struct CameraComponent::Internal {
	std::shared_ptr<RPG::Property> cameraType;

	Internal() : cameraType(std::make_unique<RPG::Property>(RPG::CameraType::Perspective, "Camera Type", "RPG::CameraType")) {}
};

CameraComponent::CameraComponent() : internal(MakeInternalPointer<Internal>()) {}

void CameraComponent::Awake() {

}

void CameraComponent::Start() {

}

void CameraComponent::Update() {

}

std::vector<std::shared_ptr<RPG::Property>> CameraComponent::GetProperties() {
	std::vector<std::shared_ptr<RPG::Property>> list = {};
	list.push_back(internal->cameraType);

	return list;
}