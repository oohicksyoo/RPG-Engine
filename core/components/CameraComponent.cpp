//
// Created by Alex on 12/27/2020.
//

#include "CameraComponent.hpp"

using RPG::CameraComponent;

struct CameraComponent::Internal {

	Internal() {}
};

CameraComponent::CameraComponent() : internal(MakeInternalPointer<Internal>()) {}

void CameraComponent::Awake() {

}

void CameraComponent::Start() {

}

void CameraComponent::Update() {

}