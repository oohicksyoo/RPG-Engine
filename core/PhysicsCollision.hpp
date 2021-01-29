//
// Created by Alex on 1/26/2021.
//

#pragma once
#include "GLMWrapper.hpp"
#include <functional>

namespace RPG {
	struct PhysicsCollision {
		public:
			float radius;
			float mass;
			glm::vec2 position;
			glm::vec2 velocity;
			bool isStatic;
			std::function<void(glm::vec2)> SetVelocity;
	};
}
