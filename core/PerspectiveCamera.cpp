//
// Created by Alex on 12/13/2020.
//

#include "PerspectiveCamera.hpp"

using RPG::PerspectiveCamera;

struct PerspectiveCamera::Internal {
	const glm::mat4 projectionMatrix;
	const glm::vec3 up;
	glm::vec3 position;
	glm::vec3 target;

	Internal(const float& width, const float& height) : projectionMatrix(glm::perspective(glm::radians(60.0f), width / height, 0.01f, 100.0f)),
														up(glm::vec3{ 0.0f, 1.0f, 0.0f }) {
	}
};

PerspectiveCamera::PerspectiveCamera(const float& width, const float& height) : internal(RPG::MakeInternalPointer<Internal>(width, height)) {}

void PerspectiveCamera::Configure(const glm::vec3& position, const glm::vec3& direction) {
	internal->position = position;
	internal->target = position - direction;
}

glm::mat4 PerspectiveCamera::GetProjectionMatrix() const {
	return internal->projectionMatrix;
}

glm::mat4 PerspectiveCamera::GetViewMatrix() const {
	return glm::lookAt(internal->position, internal->target, internal->up);
}