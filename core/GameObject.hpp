//
// Created by Alex on 12/27/2020.
//

#pragma once

#include "InternalPointer.hpp"
#include "IComponent.hpp"
#include <string>
#include <memory>

namespace RPG {
	struct GameObject {
		public:
			GameObject(std::string name = "GameObject");
			std::shared_ptr<RPG::IComponent> AddComponent(std::shared_ptr<RPG::IComponent> component);

		private:
			struct Internal;
			RPG::InternalPointer<Internal> internal;
	};
}


