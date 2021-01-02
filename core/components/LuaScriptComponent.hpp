//
// Created by Alex on 12/30/2020.
//

#pragma once

#include "../IComponent.hpp"
#include "../InternalPointer.hpp"

namespace RPG {
	struct LuaScriptComponent : public IComponent {
		public:
			LuaScriptComponent(std::string path);
			void Awake() override;
			void Start() override;
			void Update() override;
			std::string Name() { return "LuaScriptComponent";};
			std::vector<std::shared_ptr<RPG::Property>> GetProperties() override;

		private:
			struct Internal;
			RPG::InternalPointer<Internal> internal;
	};
}


