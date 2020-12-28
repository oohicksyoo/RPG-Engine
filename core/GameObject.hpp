//
// Created by Alex on 12/27/2020.
//

#pragma once

#include "InternalPointer.hpp"
#include "IComponent.hpp"
#include <string>
#include <memory>
#include <vector>

namespace RPG {
	struct GameObject {
		public:
			GameObject(std::string name = "GameObject");
			std::shared_ptr<RPG::IComponent> AddComponent(std::shared_ptr<RPG::IComponent> component);
			std::string GetGuid();
			std::string GetName();
			std::vector<std::shared_ptr<RPG::GameObject>> GetChildren();
			bool HasChildren();

		private:
			struct Internal;
			RPG::InternalPointer<Internal> internal;
	};
}


