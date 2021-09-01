//
// Created by Alex on 9/1/2021.
//

#pragma once

#include <string>

namespace RPG {
    struct IRenderable {
        public:
            IRenderable() {};
            virtual std::string GetMesh() = 0;
            //virtual std::string GetTexture() = 0;
    };
}
