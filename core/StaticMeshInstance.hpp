//
// Created by Alex on 12/13/2020.
//

#pragma once

#include "AssetInventory.hpp"
#include "GLMWrapper.hpp"
#include "InternalPointer.hpp"

namespace RPG {
	struct StaticMeshInstance {
		public:
			StaticMeshInstance(
					const RPG::Assets::StaticMesh& staticMesh,
					const RPG::Assets::Texture& texture,
					const glm::vec3& position = glm::vec3{0.0f, 0.0f, 0.0f},
					const glm::vec3& scale = glm::vec3{1.0f, 1.0f, 1.0f},
					const glm::vec3& rotationAxis = glm::vec3{0.0f, 1.0f, 0.0f},
					const float& rotationDegrees = 0.0f
			);

			void Update(const glm::mat4& projectionViewMatrix);
			void RotateBy(const float& degrees);
			RPG::Assets::StaticMesh GetMesh() const;
			RPG::Assets::Texture GetTexture() const;
			glm::mat4 GetTransformMatrix() const;

		private:
			struct Internal;
			RPG::InternalPointer<Internal> internal;
	};
}


