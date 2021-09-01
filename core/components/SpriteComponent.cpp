//
// Created by Alex on 1/4/2021.
//

#include "SpriteComponent.hpp"
#include "../Guid.hpp"

using RPG::SpriteComponent;

struct SpriteComponent::Internal {
	std::string guid;
	std::shared_ptr<RPG::Property> mesh;
	std::shared_ptr<RPG::Property> texture;

	Internal(std::string mesh, std::string texture, std::string guid)
			: guid(guid),
			  mesh(std::make_unique<RPG::Property>(mesh, "Mesh", "RPG::Resource::String")),
			  texture(std::make_unique<RPG::Property>(texture, "Texture", "RPG::Resource::String", true, "Texture")) {}
};

SpriteComponent::SpriteComponent(std::string texture, std::string guid) : internal(MakeInternalPointer<Internal>("assets/models/Quad.obj", texture, guid)) {}

void SpriteComponent::Awake() {

}

void SpriteComponent::Start() {

}

void SpriteComponent::Update(const float &delta) {

}

std::string SpriteComponent::Guid() {
	return internal->guid;
}

std::string SpriteComponent::GetMesh() {
	return std::any_cast<std::string>(internal->mesh->GetProperty());
}

std::string SpriteComponent::GetMaterial() {
    //TODO: Fill in material for sprite later
    return "";
}

std::string SpriteComponent::GetTexture() {
	return std::any_cast<std::string>(internal->texture->GetProperty());
}

std::vector<std::shared_ptr<RPG::Property>> SpriteComponent::GetProperties() {
	std::vector<std::shared_ptr<RPG::Property>> list = {};
	list.push_back(internal->texture);

	return list;
}