//
// Created by Alex on 1/1/2021.
//

#include "MeshComponent.hpp"
#include "../Guid.hpp"

using RPG::MeshComponent;

struct MeshComponent::Internal {
	std::string guid;
	std::shared_ptr<RPG::Property> mesh;
	std::shared_ptr<RPG::Property> material;

	Internal(std::string mesh, std::string material, std::string guid)
	: guid(guid),
	  mesh(std::make_unique<RPG::Property>(mesh, "Mesh", "RPG::Resource::String", true, "Model")),
      material(std::make_unique<RPG::Property>(material, "Material", "RPG::Resource::String", true, "Material")) {}
};

MeshComponent::MeshComponent(std::string mesh, std::string material, std::string guid) : internal(MakeInternalPointer<Internal>(mesh, material, guid)) {}

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

std::string MeshComponent::GetMaterial() {
    return std::any_cast<std::string>(internal->material->GetProperty());
}

std::vector<std::shared_ptr<RPG::Property>> MeshComponent::GetProperties() {
	std::vector<std::shared_ptr<RPG::Property>> list = {};
	list.push_back(internal->mesh);
    list.push_back(internal->material);

	return list;
}