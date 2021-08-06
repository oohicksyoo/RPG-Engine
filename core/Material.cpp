//
// Created by Alex on 7/23/2021.
//

#include "Material.hpp"

using RPG::Material;

struct Material::Internal {

    std::string materialName;
    int renderQueue;

    Internal(std::string materialName) : materialName(materialName) {}
};

Material::Material(std::string materialName) : internal(MakeInternalPointer<Internal>(materialName)) {}

std::string Material::GetName() {
    return internal->materialName;
}