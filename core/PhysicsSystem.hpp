//
// Created by Alex on 1/26/2021.
//

#pragma once

#include "Singleton.hpp"
#include "PhysicsCollision.hpp"
#include "components/PhysicsComponent.hpp"
#include <utility>
#include <vector>
#include <memory>
#include <string>

namespace RPG {
	struct PhysicsSystem : public Singleton<PhysicsSystem> {
		friend struct Singleton<PhysicsSystem>;

		private: const float TIME_STEP = 0.02f; //50 Times a second || 20 milliseconds

		public:
			PhysicsSystem();
			void Update(float delta);
			void Cleanup();
			void RegisterPhysicsComponent(std::shared_ptr<RPG::PhysicsComponent> component);
			void RemovePhysicsComponent(std::string guid);

		private:
			float currentTime;
			std::vector<std::shared_ptr<RPG::PhysicsComponent>> components;
			std::vector<std::shared_ptr<RPG::PhysicsComponent>> edgeComponents;
			std::vector<std::pair<RPG::PhysicsCollision, RPG::PhysicsCollision>> collisionPairs;
			std::vector<std::pair<std::shared_ptr<RPG::PhysicsComponent>, std::shared_ptr<RPG::PhysicsComponent>>> triggerPairs;
			bool DoCirclesOverlap(float x1, float y1, float r1, float x2, float y2, float r2);
			bool IsPointInCircle(float x1, float y1, float r1, float px, float py);
	};
}



