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

	Internal(RPG::Assets::StaticMesh mesh, RPG::Assets::Texture texture, std::string guid)
			: guid(guid),
			  mesh(std::make_unique<RPG::Property>(mesh, "Mesh", "RPG::Assets::StaticMesh")),
			  texture(std::make_unique<RPG::Property>(texture, "Texture", "RPG::Assets::Texture")) {}
};

SpriteComponent::SpriteComponent(RPG::Assets::Texture texture, std::string guid) : internal(MakeInternalPointer<Internal>(RPG::Assets::StaticMesh::Quad, texture, guid)) {}

void SpriteComponent::Awake() {

}

void SpriteComponent::Start() {

}

void SpriteComponent::Update(const float &delta) {

}

std::string SpriteComponent::Guid() {
	return internal->guid;
}

RPG::Assets::StaticMesh SpriteComponent::GetMesh() {
	return std::any_cast<RPG::Assets::StaticMesh>(internal->mesh->GetProperty());
}

RPG::Assets::Texture SpriteComponent::GetTexture() {
	return std::any_cast<RPG::Assets::Texture>(internal->texture->GetProperty());
}

std::vector<std::shared_ptr<RPG::Property>> SpriteComponent::GetProperties() {
	std::vector<std::shared_ptr<RPG::Property>> list = {};
	list.push_back(internal->texture);

	return list;
}