//
// Created by Alex on 12/27/2020.
//

#include "CameraComponent.hpp"
#include "../Guid.hpp"

using RPG::CameraComponent;

struct CameraComponent::Internal {
	std::string guid;
	std::shared_ptr<RPG::Property> cameraType;

	Internal() : guid(RPG::Guid::GenerateGuid()),
				 cameraType(std::make_unique<RPG::Property>(RPG::CameraType::Perspective, "Camera Type", "RPG::CameraType")) {}
};

CameraComponent::CameraComponent() : internal(MakeInternalPointer<Internal>()) {}

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