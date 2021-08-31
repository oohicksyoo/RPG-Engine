//
// Created by Alex on 8/27/2021.
//

#pragma once

#include <string>

namespace RPG {
    struct IShader {
        IShader(std::string path) = default;
        virtual ~IShader() = default;
        virtual Bind() = default;
        virtual Render(RPG::GameObjectMaterialGroup renderable) = default;
        virtual Cleanup() = default;
    };
}
