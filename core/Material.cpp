//
// Created by Alex on 7/23/2021.
//

#include "Material.hpp"

using RPG::Material;

struct Material::Internal {

    std::string materialName;
    int renderQueue;
    std::string shader;
    std::vector<std::shared_ptr<RPG::Property>> properties;

    Internal(std::string materialName, int renderQueue, std::string shader) : materialName(materialName),
                                                          renderQueue(renderQueue),
                                                          shader(shader) {}
};

Material::Material(std::string materialName, int renderQueue, std::string shader) : internal(MakeInternalPointer<Internal>(materialName, renderQueue, shader)) {}

std::string Material::GetName() {
    return internal->materialName;
}

int Material::GetRenderQueue() {
    return internal->renderQueue;
}

void Material::SetRenderQueue(int value) {
    if (value >= 0) {
        internal->renderQueue = value;
    }
}

std::string Material::GetShader() {
    return internal->shader;
}

void Material::SetShader(std::string shader) {
    internal->shader = shader;
}

std::vector<std::shared_ptr<RPG::Property>> Material::GetProperties() {
    return internal->properties;
}

void Material::SetProperties(std::vector<std::shared_ptr<RPG::Property>> properties) {
    internal->properties = properties;
}