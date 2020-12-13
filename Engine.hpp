//
// Created by Alex on 12/13/2020.
//

#pragma once

#include "InternalPointer.hpp"

namespace RPG {
    struct Engine {
        public:
    		Engine();
    		void Run();

		private:
    		struct Internal;
    		RPG::InternalPointer<Internal> internal;
    };
}
