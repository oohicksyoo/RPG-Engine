//
// Created by Alex on 8/27/2021.
//

#pragma once
#include "../Material.hpp"
#include "../Assets.hpp"
#include <memory>
#include <string>

namespace RPG {
    template <typename T>
    struct IResourceLoader;

    template <>
    struct IResourceLoader<RPG::Material> {
    public:
        std::shared_ptr<RPG::Material> Load(std::string path);
    };

    inline std::shared_ptr<RPG::Material> IResourceLoader<RPG::Material>::Load(std::string path) {
        return std::make_unique<RPG::Material>(RPG::Assets::LoadMaterial(path));
    }
}


