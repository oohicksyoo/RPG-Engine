//
// Created by Alex on 8/27/2021.
//

#pragma once

#include "GameObject.hpp"
#include <memory>

namespace RPG {
    struct GameObjectMaterialGroup {
        GameObjectMaterialGroup();

        std::shared_ptr<RPG::GameObject> gameObject;
        std::shared_ptr<RPG::Material> material;
    };
}
