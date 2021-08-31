//
// Created by Alex on 7/23/2021.
//

#pragma once

#include "InternalPointer.hpp"
#include <string>
#include <vector>

namespace RPG {
    struct Material {
        public:
            Material(std::string name, int renderQueue, std::string shader);
            std::string GetName();
            int GetRenderQueue();
            void SetRenderQueue(int value);
            std::string GetShader();
            void SetShader(std::string shader);

        private:
            struct Internal;
            RPG::InternalPointer<Internal> internal;
    };

    enum class RenderType {
        Opaque = 0,
        Transparent = 4000
    };
}
