//
// Created by Alex on 12/30/2020.
//

#pragma once

#include "../IComponent.hpp"
#include "../InternalPointer.hpp"
#include "../GameObject.hpp"
#include "../LuaWrapper.hpp"
#include "map"

namespace RPG {
	struct LuaScriptComponent : public IComponent {
		public:
			LuaScriptComponent(std::string path, std::shared_ptr<RPG::GameObject> gameObject, std::string guid = RPG::Guid::GenerateGuid());
			void Awake() override;
			void Start() override;
			void Update(const float &delta) override;
			std::string Name() { return "LuaScriptComponent";};
			std::vector<std::shared_ptr<RPG::Property>> GetProperties() override;
			std::string Guid() override;
			bool AllowMultiple() { return true; };
			void OnTriggerEnter();
			void OnTriggerStay(float delta);
			void OnTriggerExit();
			void SyncProperties();



		private:
			struct Internal;
			RPG::InternalPointer<Internal> internal;

	    public:
            static inline std::string currentMappingGuid = "";
            static inline std::map<std::string, std::vector<std::string>> mappedComponents = {};
	};
}


