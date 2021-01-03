//
// Created by Alex on 1/1/2021.
//

#include "MeshComponent.hpp"
#include "../Guid.hpp"

using RPG::MeshComponent;

struct MeshComponent::Internal {
	std::string guid;
	std::shared_ptr<RPG::Property> mesh;
	std::shared_ptr<RPG::Property> texture;

	Internal(RPG::Assets::StaticMesh mesh, RPG::Assets::Texture texture)
	: guid(RPG::Guid::GenerateGuid()),
	  mesh(std::make_unique<RPG::Property>(mesh, "Mesh", "RPG::Assets::StaticMesh")),
	  texture(std::make_unique<RPG::Property>(texture, "Texture", "RPG::Assets::Texture")) {}
};

MeshComponent::MeshComponent(RPG::Assets::StaticMesh mesh, RPG::Assets::Texture texture) : internal(MakeInternalPointer<Internal>(mesh, texture)) {}

void MeshComponent::Awake() {

}

void MeshComponent::Start() {

}

void MeshComponent::Update(const float &delta) {

}

std::string MeshComponent::Guid() {
	return internal->guid;
}

RPG::Assets::StaticMesh MeshComponent::GetMesh() {
	return std::any_cast<RPG::Assets::StaticMesh>(internal->mesh->GetProperty());
}

RPG::Assets::Texture MeshComponent::GetTexture() {
	return std::any_cast<RPG::Assets::Texture>(internal->texture->GetProperty());
}

std::vector<std::shared_ptr<RPG::Property>> MeshComponent::GetProperties() {
	std::vector<std::shared_ptr<RPG::Property>> list = {};
	list.push_back(internal->mesh);
	list.push_back(internal->texture);

	return list;
}