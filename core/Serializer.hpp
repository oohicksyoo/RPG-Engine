//
// Created by Alex on 1/5/2021.
//

#pragma once

#include "Singleton.hpp"
#include "IScene.hpp"
#include "GameObject.hpp"
#include "IComponent.hpp"
#include "nlohmann/json.hpp"
#include <unordered_map>
#include <string>

namespace RPG {
	struct Serializer : public Singleton<Serializer> {
		friend struct Singleton<Serializer>;

		public:
			std::string SaveScene(std::shared_ptr<RPG::IScene> scene, const std::string& path);
			std::unique_ptr<RPG::IScene> LoadScene(const RPG::WindowSize& frameSize, const std::string& path);
			std::unique_ptr<RPG::IScene> LoadSceneData(const RPG::WindowSize& frameSize, const std::string& sceneData);

			//Inspector Properties
			void AddPropertyLayout(std::pair<std::string, RPG::Action<std::shared_ptr<RPG::Property>>::Callback> pair);
			std::unordered_map<std::string, RPG::Action<std::shared_ptr<RPG::Property>>::Callback> GetPropertyLayouts();

			//Serialization Save
			void AddPropertySave(std::pair<std::string, std::function<nlohmann::json(std::shared_ptr<RPG::Property>)>> pair);

			//Serialization Load
			void AddComponentLoad(std::pair<std::string, std::function<std::shared_ptr<RPG::IComponent>(nlohmann::json, std::shared_ptr<RPG::GameObject> go, bool randomGuid)>> pair);

			//Default loader Types
			void LoadDefaultPropertyTypes();

			nlohmann::json SaveGameObject(std::shared_ptr<RPG::GameObject> gameObject);
			std::shared_ptr<RPG::GameObject> LoadGameObject(nlohmann::json j, bool randomGuid = false);

		private:
			nlohmann::json SaveComponent(std::shared_ptr<RPG::IComponent> component);
			nlohmann::json SavePropertyValue(std::shared_ptr<RPG::Property> property);
			void LoadDefaultSavePropertyTypes();

			std::shared_ptr<RPG::IComponent> LoadComponent(nlohmann::json j, std::shared_ptr<RPG::GameObject> go, bool randomGuid = false);
			void LoadDefaultLoadComponentTypes();

			//Inspector Properties
			std::unordered_map<std::string, RPG::Action<std::shared_ptr<RPG::Property>>::Callback> variableTypes;
			std::unordered_map<std::string, std::function<nlohmann::json(std::shared_ptr<RPG::Property>)>> propertyToJsonTypes;
			std::unordered_map<std::string, std::function<std::shared_ptr<RPG::IComponent>(nlohmann::json, std::shared_ptr<RPG::GameObject> go, bool randomGuid)>> jsonTypesToComponent;


	};

}


