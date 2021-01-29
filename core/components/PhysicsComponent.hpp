//
// Created by Alex on 1/26/2021.
//

#pragma once

#include "../IComponent.hpp"
#include "../InternalPointer.hpp"
#include "../GLMWrapper.hpp"
#include "../Guid.hpp"
#include "TransformComponent.hpp"
#include "../PhysicsCollision.hpp"
#include <string>
#include <memory>

namespace RPG {

	enum PhysicsShape {
		Circle,
		Capsule
	};

	struct PhysicsComponent : public IComponent {
		public:
			PhysicsComponent(std::shared_ptr<RPG::TransformComponent> transform, std::string guid = RPG::Guid::GenerateGuid());
			void Awake() override;
			void Start() override;
			void Update(const float &delta) override;
			std::string Name() { return "PhysicsComponent";};
			std::vector<std::shared_ptr<RPG::Property>> GetProperties() override;
			std::string Guid() override;
			bool AllowMultiple() { return false; };
			bool IsStatic();
			float GetMass();
			RPG::PhysicsShape GetShape();
			float GetDiameter();
			bool IsTrigger();
			glm::vec2 GetVelocity();
			glm::vec2 GetAcceleration();
			glm::vec2 GetPosition();
			glm::vec2 GetWorldPosition();
			RPG::PhysicsCollision GetCollisionData();
			glm::vec2 GetStartPosition();
			glm::vec2 GetEndPosition();
			void SetIsStatic(bool value);
			void SetIsTrigger(bool value);
			void SetMass(float value);
			void SetDiameter(float value);
			void SetPhysicsShape(RPG::PhysicsShape shape);
			void SetPosition(glm::vec2 position);
			void SetVelocity(glm::vec2 value);
			void SetAcceleration(glm::vec2 value);
			void SetStartPosition(glm::vec2 value);
			void SetEndPosition(glm::vec2 value);


		private:
			struct Internal;
			RPG::InternalPointer<Internal> internal;
	};
}