//
// Created by Alex on 12/27/2020.
//

#pragma once

#include "InternalPointer.hpp"
#include "GameObject.hpp"
#include "components/TransformComponent.hpp"
#include <string>
#include <vector>

namespace RPG {
	struct Hierarchy {
		public:
			Hierarchy();
			void Add(std::shared_ptr<RPG::GameObject> gameObject, std::shared_ptr<RPG::GameObject> parent = nullptr);
			void Remove(std::shared_ptr<RPG::GameObject> gameObject);
			std::shared_ptr<RPG::GameObject> GetGameObjectByGuid(std::string guid);
			std::vector<std::shared_ptr<RPG::GameObject>> GetHierarchy();

		private:
			struct Internal;
			RPG::InternalPointer<Internal> internal;

	};
}


