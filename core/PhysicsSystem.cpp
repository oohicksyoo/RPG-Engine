//
// Created by Alex on 1/26/2021.
//

#include "PhysicsSystem.hpp"
#include "Log.hpp"

using RPG::PhysicsSystem;

PhysicsSystem::PhysicsSystem() {

}

void PhysicsSystem::Update(float delta) {
	bool runPhysicsStep = false;
	currentTime += delta;
	if (currentTime >= TIME_STEP) {
		currentTime = 0;
		runPhysicsStep = true;
	}

	if (!runPhysicsStep) {
		return;
	}

	collisionPairs.clear();

	//Runs Physics Loop
	for (auto component : components) {
		if (component->IsStatic() || component->IsTrigger()) continue;

		auto position = component->GetPosition();
		auto velocity = component->GetVelocity();
		auto acceleration = component->GetAcceleration();

		//Apply Drag Here
		acceleration = -velocity * 1.6f;

		//Update Velocity
		velocity += acceleration * delta;
		position += velocity;

		//Clamp Velocity
		if (std::abs(velocity.x * velocity.x + velocity.y * velocity.y) < 0.001f) {
			velocity = glm::vec2{0, 0};
		}

		//Clamp acceleration
		if (acceleration.x < 0) {
			acceleration.x = 0;
		}

		if (acceleration.y < 0) {
			acceleration.y = 0;
		}

		component->SetPosition(position);
		component->SetAcceleration(acceleration);
		component->SetVelocity(velocity);
	}

	//Static Collisions
	for (auto component : components) {
		if (component->IsStatic() || component->IsTrigger()) continue;

		//Check circles against Edges/Capsules
		for (auto edgeComponent : edgeComponents) {

		}

		//Check against other balls
		for (auto otherComponent : components) {
			if (component->Guid() == otherComponent->Guid()) continue;

			auto cPosition = component->GetPosition();
			auto cRadius = component->GetDiameter() * 0.5f;
			auto oPosition = otherComponent->GetPosition();
			auto oRadius = otherComponent->GetDiameter() * 0.5f;
			auto oStatic = otherComponent->IsStatic();
			auto oTrigger = otherComponent->IsTrigger();

			if (DoCirclesOverlap(cPosition.x, cPosition.y, cRadius, oPosition.x, oPosition.y, oRadius)) {

				//Collision Occured
				if (!oStatic && !oTrigger) {
					collisionPairs.push_back(std::make_pair(component->GetCollisionData(), otherComponent->GetCollisionData()));
				} else if (oStatic && !oTrigger) {
					auto cd = otherComponent->GetCollisionData();
					cd.mass *= 0.8f;
					cd.velocity = -component->GetVelocity();
					collisionPairs.push_back(std::make_pair(component->GetCollisionData(), cd));
				} else if (oTrigger) {
					//Hand Trigger for this if it hasnt been fired before
				}

				//Distance between ball centers
				float distance = std::sqrt(
					(cPosition.x - oPosition.x) * (cPosition.x - oPosition.x) +
						(cPosition.y - oPosition.y) * (cPosition.y - oPosition.y));

				if (!oStatic) {
					//Calculate displacement required
					float overlap = 0.5f * (distance - cRadius - oRadius);

					glm::vec2 offset = glm::vec2{
							overlap * (cPosition.x - oPosition.x) / distance,
							overlap * (cPosition.y - oPosition.y) / distance
					};

					//Displace my current circle away from the collision
					cPosition -= offset;
					oPosition += offset;

					//Set Positions
					component->SetPosition(cPosition);
					otherComponent->SetPosition(oPosition);
				} else {
					//Calculate displacement required
					float overlap = 1.0f * (distance - cRadius - oRadius);

					glm::vec2 offset = glm::vec2{
							overlap * (cPosition.x - oPosition.x) / distance,
							overlap * (cPosition.y - oPosition.y) / distance
					};

					//Displace my current circle away from the collision
					cPosition -= offset;

					//Set Positions
					component->SetPosition(cPosition);
				}
			}
		}
	}

	//Work out dynamic Collisions
	for (auto pair : collisionPairs) {
		auto first = pair.first;
		auto second = pair.second;

		//Distance between centers
		float distance = std::sqrt(
				(first.position.x - second.position.x) * (first.position.x - second.position.x) +
				(first.position.y - second.position.y) * (first.position.y - second.position.y));

		//Normal
		glm::vec2 normal{
				(second.position.x - first.position.x) / distance,
				(second.position.y - first.position.y) / distance
		};

		//Tangent
		glm::vec2 tangent{-normal.y, normal.x};

		//Dot Product Tangent
		glm::vec2 dotProductTangent{
			first.velocity.x * tangent.x + first.velocity.y * tangent.y,
			second.velocity.x * tangent.x + second.velocity.y * tangent.y
		};

		//Dot Product Normal
		glm::vec2 dotProductNormal{
				first.velocity.x * normal.x + first.velocity.y * normal.y,
				second.velocity.x * normal.x + second.velocity.y * normal.y
		};

		//Conservation of Momentum
		glm::vec2 momentum{
				(dotProductNormal.x * (first.mass - second.mass) + 2.0f * second.mass * dotProductNormal.y) / (first.mass + second.mass),
				(dotProductNormal.y * (second.mass - first.mass) + 2.0f * first.mass * dotProductNormal.x) / (first.mass + second.mass)
 		};

		//Update circle velocities
		if (!first.isStatic) {
			first.SetVelocity(glm::vec2{
				tangent.x * dotProductTangent.x + normal.x * momentum.x,
				tangent.y * dotProductTangent.x + normal.y * momentum.x
			});
		} else {
			first.SetVelocity(glm::vec2{0, 0});
		}

		if (!second.isStatic) {
			second.SetVelocity(glm::vec2{
					tangent.x * dotProductTangent.y + normal.x * momentum.y,
					tangent.y * dotProductTangent.y + normal.y * momentum.y
			});
		} else {
			second.SetVelocity(glm::vec2{0, 0});
		}
	}
}

void PhysicsSystem::Cleanup() {
	components.clear();
	edgeComponents.clear();
}

void PhysicsSystem::RegisterPhysicsComponent(std::shared_ptr<RPG::PhysicsComponent> component) {
	components.push_back(component);
}

void PhysicsSystem::RemovePhysicsComponent(std::string guid) {
	int index = -1;
	for (int i = 0; i < components.size(); ++i) {
		if (components[i]->Guid() == guid) {
			index = i;
		}
	}

	if (index != -1) {
		components.erase(components.begin() + index);
	}
}

bool PhysicsSystem::DoCirclesOverlap(float x1, float y1, float r1, float x2, float y2, float r2) {

	return std::abs((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2)) <= (r1 + r2) * (r1 + r2);
}

bool PhysicsSystem::IsPointInCircle(float x1, float y1, float r1, float px, float py) {
	return std::abs((x1 - px) * (x1 - px) + (y1 - py) * (y1 - py)) < (r1 * r1);
}