//
// Created by Alex on 1/26/2021.
//

#include "PhysicsComponent.hpp"
#include "../GLMWrapper.hpp"

using RPG::PhysicsComponent;

struct PhysicsComponent::Internal {
	std::string guid;
	std::shared_ptr<RPG::TransformComponent> transform;
	std::shared_ptr<RPG::Property> isStatic;
	std::shared_ptr<RPG::Property> isTrigger;
	std::shared_ptr<RPG::Property> mass;
	std::shared_ptr<RPG::Property> shape;
	std::shared_ptr<RPG::Property> diameter;
	glm::vec2 velocity;
	glm::vec2 acceleration;

	Internal(std::shared_ptr<RPG::TransformComponent> transform, std::string guid)
			: guid(guid),
			  transform(transform),
			  isStatic(std::make_unique<RPG::Property>(false, "Is Static", "bool")),
			  isTrigger(std::make_unique<RPG::Property>(false, "Is Trigger", "bool")),
			  mass(std::make_unique<RPG::Property>(1.0f, "Mass", "float")),
			  shape(std::make_unique<RPG::Property>(RPG::PhysicsShape::Circle, "Physics Shape", "RPG::PhysicsShape")),
			  diameter(std::make_unique<RPG::Property>(1.0f, "Diameter", "float")) {}
};

PhysicsComponent::PhysicsComponent(std::shared_ptr<RPG::TransformComponent> transform, std::string guid) : internal(MakeInternalPointer<Internal>(transform, guid)) {}

void PhysicsComponent::Awake() {

}

void PhysicsComponent::Start() {

}

void PhysicsComponent::Update(const float &delta) {

}

std::string PhysicsComponent::Guid() {
	return internal->guid;
}

bool PhysicsComponent::IsStatic() {
	return std::any_cast<bool>(internal->isStatic->GetProperty());
}

float PhysicsComponent::GetDiameter() {
	return std::any_cast<float>(internal->diameter->GetProperty());
}

float PhysicsComponent::GetMass() {
	return std::any_cast<float>(internal->mass->GetProperty());
}

bool PhysicsComponent::IsTrigger() {
	return std::any_cast<bool>(internal->isTrigger->GetProperty());
}

glm::vec2 PhysicsComponent::GetPosition() {
	auto position = internal->transform->GetPosition();
	return glm::vec2{position.x, position.z};
}

glm::vec2 PhysicsComponent::GetVelocity() {
	return internal->velocity;
}

glm::vec2 PhysicsComponent::GetAcceleration() {
	return internal->acceleration;
}

RPG::PhysicsCollision PhysicsComponent::GetCollisionData() {
	RPG::PhysicsCollision c = RPG::PhysicsCollision();
	c.velocity = internal->velocity;
	c.position = GetPosition();
	c.mass = GetMass();
	c.radius = GetDiameter() * 0.5f;
	c.isStatic = IsStatic();
	return c;
}

void PhysicsComponent::SetIsStatic(bool value) {
	internal->isStatic->SetProperty(value);
}

void PhysicsComponent::SetIsTrigger(bool value) {
	internal->isTrigger->SetProperty(value);
}

void PhysicsComponent::SetMass(float value) {
	internal->mass->SetProperty(value);
}

void PhysicsComponent::SetDiameter(float value) {
	internal->diameter->SetProperty(value);
}

void PhysicsComponent::SetPhysicsShape(RPG::PhysicsShape shape) {
	internal->shape->SetProperty(shape);
}

void PhysicsComponent::SetPosition(glm::vec2 position) {
	auto p = internal->transform->GetPosition();
	internal->transform->SetPosition(glm::vec3{position.x, p.y, position.y});
}

void PhysicsComponent::SetVelocity(glm::vec2 value) {
	internal->velocity = value;
}

void PhysicsComponent::SetAcceleration(glm::vec2 value) {
	internal->acceleration = value;
}

std::vector<std::shared_ptr<RPG::Property>> PhysicsComponent::GetProperties() {
	std::vector<std::shared_ptr<RPG::Property>> list = {};
	list.push_back(internal->isStatic);
	list.push_back(internal->isTrigger);
	list.push_back(internal->mass);
	list.push_back(internal->shape);
	list.push_back(internal->diameter);

	return list;
}