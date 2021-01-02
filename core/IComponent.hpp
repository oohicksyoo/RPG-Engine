//
// Created by Alex on 12/27/2020.
//

#pragma once

#include "Property.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <any>

namespace RPG {
	struct IComponent {
		public:
			IComponent() {};
			virtual void Awake() = 0;
			virtual void Start() = 0;
			virtual void Update() = 0;
			virtual std::string Name() = 0;
			virtual std::vector<std::shared_ptr<RPG::Property>> GetProperties() = 0;
	};
}
