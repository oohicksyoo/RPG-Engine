//
// Created by Alex on 1/26/2021.
//

#include "PhysicsSystem.hpp"
#include "../application/ApplicationStats.hpp"
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

		auto position = component->GetWorldPosition();
		auto velocity = component->GetVelocity();
		auto acceleration = component->GetAcceleration();

		//Apply Drag Here
		acceleration = -velocity * 1.6f;

		//Update Velocity
		velocity += acceleration * delta;
		position += glm::vec2{velocity.x, -velocity.y};

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

		//TODO: Setting World Position is not correct causing pinging between two locations
		component->SetWorldPosition(position);
		component->SetAcceleration(acceleration);
		component->SetVelocity(velocity);
	}

	//Check if Triggers still exist
	//TODO: If need to erase trigger call OnTriggerExit
	//Else OnTriggerStay
	std::vector<std::pair<std::shared_ptr<RPG::PhysicsComponent>, std::shared_ptr<RPG::PhysicsComponent>>> tPairs;
	for (auto pair : triggerPairs) {
		auto first = pair.first;
		auto second = pair.second;

		auto cPosition = first->GetWorldPosition();
		auto cRadius = first->GetDiameter() * 0.5f;
		auto oPosition = second->GetWorldPosition();
		auto oRadius = second->GetDiameter() * 0.5f;

		if (DoCirclesOverlap(cPosition.x, cPosition.y, cRadius, oPosition.x, oPosition.y, oRadius)) {
			first->OnTriggerStay(RPG::ApplicationStats::GetInstance().GetDelta());
			tPairs.push_back(pair);
		} else {
			first->OnTriggerExit();
		}
	}
	triggerPairs = tPairs;

	//Static Collisions
	for (auto component : components) {
		if (component->IsStatic() || component->IsTrigger()) continue;

		//Check circles against Edges/Capsules
		for (auto edgeComponent : edgeComponents) {
			auto cPosition = component->GetWorldPosition();
			auto cRadius = component->GetDiameter() * 0.5f;
			auto cMass = component->GetMass();
			auto cVelocity = component->GetVelocity();

			auto globalPosition = edgeComponent->GetWorldPosition();
			auto startPosition = globalPosition + edgeComponent->GetStartPosition();
			auto endPosition = globalPosition + edgeComponent->GetEndPosition();
			auto eRadius = edgeComponent->GetDiameter() * 0.5f;


			//Check that line formed by the velocity vector, intersects with line segment
			auto line1 = endPosition - startPosition;
			auto line2 = cPosition - startPosition;

			float edgeLength = line1.x * line1.x + line1.y * line1.y;

			// This is nifty - It uses the DP of the line segment vs the line to the object, to work out
			// how much of the segment is in the "shadow" of the object vector. The min and max clamp
			// this to lie between 0 and the line segment length, which is then normalised. We can
			// use this to calculate the closest point on the line segment
			float t = std::max(0.0f, std::min(edgeLength, (line1.x * line2.x + line1.y * line2.y))) / edgeLength;

			//Closest Point
			glm::vec2 closestPoint{
				startPosition.x + t * line1.x,
				startPosition.y + t * line1.y
			};

			//Now that we know the closest point we can check if the circle has collided
			float distance = std::sqrt((cPosition.x - closestPoint.x) * (cPosition.x - closestPoint.x) +
											   (cPosition.y - closestPoint.y) * (cPosition.y - closestPoint.y));

			//Check Collision
			if (distance <= (cRadius + eRadius)) {
				auto data = edgeComponent->GetCollisionData();
				data.mass = cMass * 0.8f;
				data.position = closestPoint;
				data.velocity = -cVelocity;

				collisionPairs.push_back(std::make_pair(component->GetCollisionData(), data));

				//Calculate displacement Required
				float overlap = 1.0f * (distance - cRadius - data.radius);

				//Displace circle away from collision
				cPosition -= glm::vec2{
					overlap * (cPosition.x - closestPoint.x) / distance,
					overlap * (cPosition.y - closestPoint.y) / distance
				};

				//Set Positions
				component->SetWorldPosition(cPosition);
			}
		}

		//Check against other balls
		for (auto otherComponent : components) {
			if (component->Guid() == otherComponent->Guid()) continue;

			auto cPosition = component->GetWorldPosition();
			auto cRadius = component->GetDiameter() * 0.5f;
			auto oPosition = otherComponent->GetWorldPosition();
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
					//Add to a trigger list
					auto pair = std::make_pair(component, otherComponent);

					if (std::find(triggerPairs.begin(), triggerPairs.end(), pair) == triggerPairs.end()) {
						component->OnTriggerEnter();
						triggerPairs.push_back(pair);
					}
					continue;
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
					component->SetWorldPosition(cPosition);
					otherComponent->SetWorldPosition(oPosition);
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
					component->SetWorldPosition(cPosition);
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
	if (component->GetShape() == RPG::PhysicsShape::Circle) {
		components.push_back(component);
	} else {
		edgeComponents.push_back(component);
	}

}

void PhysicsSystem::RemovePhysicsComponent(std::string guid) {
	int index = -1;
	for (int i = 0; i < components.size(); ++i) {
		if (components[i]->Guid() == guid) {
			index = i;
		}
	}

	int edgeIndex = -1;
	for (int i = 0; i < edgeComponents.size(); ++i) {
		if (edgeComponents[i]->Guid() == guid) {
			edgeIndex = i;
		}
	}

	if (index != -1) {
		components.erase(components.begin() + index);
	}

	if (edgeIndex != -1) {
		edgeComponents.erase(edgeComponents.begin() + edgeIndex);
	}
}

bool PhysicsSystem::DoCirclesOverlap(float x1, float y1, float r1, float x2, float y2, float r2) {

	return std::abs((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2)) <= (r1 + r2) * (r1 + r2);
}

bool PhysicsSystem::IsPointInCircle(float x1, float y1, float r1, float px, float py) {
	return std::abs((x1 - px) * (x1 - px) + (y1 - py) * (y1 - py)) < (r1 * r1);
}