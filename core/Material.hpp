//
// Created by Alex on 7/23/2021.
//

#pragma once

#include "InternalPointer.hpp"
#include <string>

namespace RPG {
    struct Material {
        public:
            Material(std::string shaderName);
            std::string GetName();

        private:
            struct Internal;
            RPG::InternalPointer<Internal> internal;
    };
}
