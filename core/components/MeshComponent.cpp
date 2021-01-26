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

	Internal(std::string mesh, std::string texture, std::string guid)
	: guid(guid),
	  mesh(std::make_unique<RPG::Property>(mesh, "Mesh", "std::string", true, "Model")),
	  texture(std::make_unique<RPG::Property>(texture, "Texture", "std::string", true, "Texture")) {}
};

MeshComponent::MeshComponent(std::string mesh, std::string texture, std::string guid) : internal(MakeInternalPointer<Internal>(mesh, texture, guid)) {}

void MeshComponent::Awake() {

}

void MeshComponent::Start() {

}

void MeshComponent::Update(const float &delta) {

}

std::string MeshComponent::Guid() {
	return internal->guid;
}

std::string MeshComponent::GetMesh() {
	return std::any_cast<std::string>(internal->mesh->GetProperty());
}

std::string MeshComponent::GetTexture() {
	return std::any_cast<std::string>(internal->texture->GetProperty());
}

std::vector<std::shared_ptr<RPG::Property>> MeshComponent::GetProperties() {
	std::vector<std::shared_ptr<RPG::Property>> list = {};
	list.push_back(internal->mesh);
	list.push_back(internal->texture);

	return list;
}