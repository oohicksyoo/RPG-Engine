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
	std::shared_ptr<RPG::Property> startPosition;
	std::shared_ptr<RPG::Property> endPosition;
	glm::vec2 velocity;
	glm::vec2 acceleration;
	std::function<std::vector<std::shared_ptr<RPG::IComponent>>()> getLuaScripts;

	Internal(std::shared_ptr<RPG::TransformComponent> transform, std::function<std::vector<std::shared_ptr<RPG::IComponent>>()> getLuaScripts, std::string guid)
			: guid(guid),
			  transform(transform),
			  getLuaScripts(getLuaScripts),
			  isStatic(std::make_unique<RPG::Property>(false, "Is Static", "bool")),
			  isTrigger(std::make_unique<RPG::Property>(false, "Is Trigger", "bool")),
			  mass(std::make_unique<RPG::Property>(1.0f, "Mass", "float")),
			  shape(std::make_unique<RPG::Property>(RPG::PhysicsShape::Circle, "Physics Shape", "RPG::PhysicsShape")),
			  diameter(std::make_unique<RPG::Property>(1.0f, "Diameter", "float")),
			  startPosition(std::make_unique<RPG::Property>(glm::vec2{0, 0}, "Start Position (Capsule)", "glm::vec2")),
			  endPosition(std::make_unique<RPG::Property>(glm::vec2{0, 0}, "End Position (Capsule)", "glm::vec2")),
			  velocity(glm::vec2{0, 0}),
			  acceleration(glm::vec2{0, 0}) {}
};

PhysicsComponent::PhysicsComponent(std::shared_ptr<RPG::TransformComponent> transform, std::function<std::vector<std::shared_ptr<RPG::IComponent>>()> getLuaScripts, std::string guid) : internal(MakeInternalPointer<Internal>(transform, getLuaScripts, guid)) {}

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

RPG::PhysicsShape PhysicsComponent::GetShape() {
	return std::any_cast<RPG::PhysicsShape>(internal->shape->GetProperty());
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

glm::vec2 PhysicsComponent::GetWorldPosition() {
	auto pos = internal->transform->GetWorldPosition();
	return glm::vec2{pos.x, pos.z};
}

glm::vec2 PhysicsComponent::GetVelocity() {
	return internal->velocity;
}

glm::vec2 PhysicsComponent::GetAcceleration() {
	return internal->acceleration;
}

glm::vec2 PhysicsComponent::GetStartPosition() {
	return std::any_cast<glm::vec2>(internal->startPosition->GetProperty());
}

glm::vec2 PhysicsComponent::GetEndPosition() {
	return std::any_cast<glm::vec2>(internal->endPosition->GetProperty());
}

RPG::PhysicsCollision PhysicsComponent::GetCollisionData() {
	RPG::PhysicsCollision c = RPG::PhysicsCollision();
	c.velocity = internal->velocity;
	c.position = GetPosition();
	c.mass = GetMass();
	c.radius = GetDiameter() * 0.5f;
	c.isStatic = IsStatic();
	c.SetVelocity = [this](glm::vec2 velocity) {
		SetVelocity(velocity);
	};
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

void PhysicsComponent::SetWorldPosition(glm::vec2 position) {
	auto pos = internal->transform->GetWorldPosition();
	internal->transform->SetWorldPosition(glm::vec3{position.x, pos.y, position.y});
}

void PhysicsComponent::SetVelocity(glm::vec2 value) {
	internal->velocity = value;
}

void PhysicsComponent::SetAcceleration(glm::vec2 value) {
	internal->acceleration = value;
}

void PhysicsComponent::SetStartPosition(glm::vec2 value) {
	internal->startPosition->SetProperty(value);
}

void PhysicsComponent::SetEndPosition(glm::vec2 value) {
	internal->endPosition->SetProperty(value);
}

void PhysicsComponent::OnTriggerEnter() {
	for (auto c : internal->getLuaScripts()) {
		std::shared_ptr<RPG::LuaScriptComponent> lsc = std::dynamic_pointer_cast<RPG::LuaScriptComponent>(c);
		lsc->OnTriggerEnter();
	}
}

void PhysicsComponent::OnTriggerStay(float delta) {
	for (auto c : internal->getLuaScripts()) {
		std::shared_ptr<RPG::LuaScriptComponent> lsc = std::dynamic_pointer_cast<RPG::LuaScriptComponent>(c);
		lsc->OnTriggerStay(delta);
	}
}

void PhysicsComponent::OnTriggerExit() {
	for (auto c : internal->getLuaScripts()) {
		std::shared_ptr<RPG::LuaScriptComponent> lsc = std::dynamic_pointer_cast<RPG::LuaScriptComponent>(c);
		lsc->OnTriggerExit();
	}
}

std::vector<std::shared_ptr<RPG::Property>> PhysicsComponent::GetProperties() {
	std::vector<std::shared_ptr<RPG::Property>> list = {};
	list.push_back(internal->isStatic);
	list.push_back(internal->isTrigger);
	list.push_back(internal->mass);
	list.push_back(internal->shape);
	list.push_back(internal->diameter);
	list.push_back(internal->startPosition);
	list.push_back(internal->endPosition);

	return list;
}