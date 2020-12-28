//
// Created by Alex on 12/27/2020.
//

#pragma once

#include "InternalPointer.hpp"
#include "GameObject.hpp"
#include "Components/TransformComponent.hpp"
#include <string>

namespace RPG {
	struct Hierarchy {
		public:
			Hierarchy();
			void Add(RPG::GameObject gameObject, RPG::TransformComponent parent = nullptr);
			RPG::GameObject GetGameObjectByGuid(std::string guid);

		private:
			struct Internal;
			RPG::InternalPointer<Internal> internal;

	};
}


