//
// Created by Alex on 12/13/2020.
//

#pragma once

#include "GLMWrapper.hpp"
#include "InternalPointer.hpp"

namespace RPG {
	struct PerspectiveCamera {
		public:
			PerspectiveCamera(const float& width, const float& height);
			void Configure(const glm::vec3& position, const glm::vec3& direction);
			glm::mat4 GetProjectionMatrix() const;
			glm::mat4 GetViewMatrix() const;

		private:
			struct Internal;
			RPG::InternalPointer<Internal> internal;
	};
}


