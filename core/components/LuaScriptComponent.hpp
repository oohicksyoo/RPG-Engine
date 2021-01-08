//
// Created by Alex on 12/30/2020.
//

#pragma once

#include "../IComponent.hpp"
#include "../InternalPointer.hpp"

namespace RPG {
	struct LuaScriptComponent : public IComponent {
		public:
			LuaScriptComponent(std::string path, std::string guid = RPG::Guid::GenerateGuid());
			void Awake() override;
			void Start() override;
			void Update(const float &delta) override;
			std::string Name() { return "LuaScriptComponent";};
			std::vector<std::shared_ptr<RPG::Property>> GetProperties() override;
			std::string Guid() override;
			bool AllowMultiple() { return true; };

		private:
			struct Internal;
			RPG::InternalPointer<Internal> internal;
	};
}


