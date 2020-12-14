//
// Created by Alex on 12/13/2020.
//

#include "StaticMeshInstance.hpp"

using RPG::StaticMeshInstance;

struct StaticMeshInstance::Internal {
	const RPG::Assets::StaticMesh mesh;
	const RPG::Assets::Texture texture;
	const glm::mat4 identity;

	glm::vec3 position;
	glm::vec3 scale;
	glm::vec3 rotationAxis;
	float rotationDegrees;
	glm::mat4 transformMatrix;

	Internal(const RPG::Assets::StaticMesh& mesh,
			const RPG::Assets::Texture& texture,
			const glm::vec3& position,
			const glm::vec3& scale,
			const glm::vec3& rotationAxis,
			const float& rotationDegrees
	) : mesh(mesh),
		texture(texture),
		identity(glm::mat4{1.0f}),
		position(position),
		scale(scale),
		rotationAxis(rotationAxis),
		rotationDegrees(rotationDegrees),
		transformMatrix(identity) {}

	void Update(const glm::mat4& projectionViewMatrix) {
		transformMatrix = projectionViewMatrix *
						  glm::translate(identity, position) *
						  glm::rotate(identity, glm::radians(rotationDegrees), rotationAxis) *
						  glm::scale(identity, scale);
	}

	void RotateBy(const float& degrees) {
		rotationDegrees += degrees;

		if (rotationDegrees > 360.0f) {
			rotationDegrees -= 360.0f;
		} else if (rotationDegrees < -360.0f) {
			rotationDegrees += 360.0f;
		}
	}
};

StaticMeshInstance::StaticMeshInstance(
		const RPG::Assets::StaticMesh& mesh,
		const RPG::Assets::Texture& texture,
		const glm::vec3& position,
		const glm::vec3& scale,
		const glm::vec3& rotationAxis,
		const float& rotationDegrees
) : internal(RPG::MakeInternalPointer<Internal>(
		mesh,
		texture,
		position,
		scale,
		rotationAxis,
		rotationDegrees
)) {}

void StaticMeshInstance::Update(const glm::mat4& projectionViewMatrix) {
	internal->Update(projectionViewMatrix);
}

void StaticMeshInstance::RotateBy(const float& degrees) {
	internal->RotateBy(degrees);
}

RPG::Assets::StaticMesh StaticMeshInstance::GetMesh() const {
	return internal->mesh;
}

RPG::Assets::Texture StaticMeshInstance::GetTexture() const {
	return internal->texture;
}

glm::mat4 StaticMeshInstance::GetTransformMatrix() const {
	return internal->transformMatrix;
}