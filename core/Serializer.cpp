//
// Created by Alex on 1/5/2021.
//

#include "Serializer.hpp"
#include "Scene.hpp"

#include "Hierarchy.hpp"
#include "Assets.hpp"
#include "Log.hpp"
#include "../application/ApplicationStats.hpp"
#include "components/MeshComponent.hpp"
#include "components/SpriteComponent.hpp"
#include "components/CameraComponent.hpp"
#include "components/LuaScriptComponent.hpp"

using RPG::Serializer;
using RPG::Assets::Pipeline;
using RPG::Assets::StaticMesh;
using RPG::Assets::Texture;
using json = nlohmann::json;

//Saving
void Serializer::SaveScene(std::shared_ptr<RPG::IScene> scene, const std::string& path) {
	auto hierarchy = scene->GetHierarchy();
	json j;
	auto js = json::object();

	js.push_back({"Major", 1});
	js.push_back({"Minor", 0});
	js.push_back({"Build", 0});
	j["Information"] = {
		{"Name", "Scene"},
		{"Guid", scene->GetGuid()},
		{"Version", js}
	};

	for (auto gameObject : hierarchy->GetHierarchy()) {
		j["Hierarchy"].push_back(SaveGameObject(gameObject));
	}

	RPG::Log("Serializer - Save", j.dump());


	RPG::Assets::SaveTextFile(j.dump(), path);
}

nlohmann::json Serializer::SaveGameObject(std::shared_ptr<RPG::GameObject> gameObject) {
	json obj = json::object();

	obj.push_back({"Name", gameObject->GetName()});
	obj.push_back({"Guid", gameObject->GetGuid()});

	json components = json::array();
	for (auto component : gameObject->GetComponents()) {
		obj["Components"].push_back(SaveComponent(component));
	}

	for (auto child : gameObject->GetChildren()) {
		obj["Children"].push_back(SaveGameObject(child));
	}

	return obj;
}

nlohmann::json Serializer::SaveComponent(std::shared_ptr<RPG::IComponent> component) {
	json obj = json::object();

	obj.push_back({"Name", component->Name()});
	obj.push_back({"Guid", component->Guid()});

	for (auto property : component->GetProperties()) {
		json prop = json::object();
		prop.push_back({"Name", property->GetName()});
		prop.push_back({"Guid", property->GetGuid()});
		prop.push_back({"Type", property->GetType()});
		prop.merge_patch(SavePropertyValue(property));

		obj["Properties"].push_back(prop);
	}

	return obj;
}

//TODO: Similar to displaying this value in the editor we need a map of <Type, Func<VALUE>>
//The map should actually just return the json::object with the one key of Value so the mapping can exist
nlohmann::json Serializer::SavePropertyValue(std::shared_ptr<RPG::Property> property) {
	json obj = json::object();

	for (auto const& [key, value] : propertyToJsonTypes) {
		if (key == property->GetType()) {
			return value(property);
		}
	}

	return obj;
}

//Loading
std::unique_ptr<RPG::IScene> Serializer::LoadScene(const RPG::WindowSize& frameSize, const std::string& path) {
	json j = json::parse(RPG::Assets::LoadTextFile(path)); //TODO: Validate path exists
	RPG::Log("Serializer - Load", j.dump());

	std::unique_ptr<RPG::IScene> scene = std::make_unique<RPG::Scene>(RPG::Scene(frameSize, j["Information"]["Guid"].get<std::string>()));
	auto hierarchy = scene->GetHierarchy();

	for (auto [key, gameObject] : j["Hierarchy"].items()) {
		hierarchy->Add(LoadGameObject(gameObject));
	}

	return scene;
}

std::shared_ptr<RPG::GameObject> Serializer::LoadGameObject(nlohmann::json j) {
	RPG::Log("Serializer", j["Name"].get<std::string>() + "|" + j["Guid"].get<std::string>());
	std::shared_ptr<RPG::GameObject> go = std::make_unique<RPG::GameObject>(RPG::GameObject(j["Name"].get<std::string>(), j["Guid"].get<std::string>()));

	for (auto [key, gameObject] : j["Components"].items()) {
		go->AddComponent(LoadComponent(gameObject, go));
	}

	for (auto [key, gameObject] : j["Children"].items()) {
		go->AddChild(LoadGameObject(gameObject));
	}

	return go;
}

std::shared_ptr<RPG::IComponent> Serializer::LoadComponent(nlohmann::json j, std::shared_ptr<RPG::GameObject> go) {
	std::string componentName = j["Name"].get<std::string>();
	for (auto const& [key, value] : jsonTypesToComponent) {
		if (key == componentName) {
			return value(j, go);
		}
	}
}

//Inspector Property
void Serializer::AddPropertyLayout(
		std::pair<std::string, RPG::Action<std::shared_ptr<RPG::Property>>::Callback> pair) {
	//TODO: In future double check the key is not already used, maybe we use that to override the default methods
	variableTypes.insert(pair);
}

std::unordered_map<std::string, RPG::Action<std::shared_ptr<RPG::Property>>::Callback> Serializer::GetPropertyLayouts() {
	return variableTypes;
}

void Serializer::AddPropertySave(std::pair<std::string, std::function<nlohmann::json(std::shared_ptr<RPG::Property>)>> pair) {
	propertyToJsonTypes.insert(pair);
}

void Serializer::AddComponentLoad(
		std::pair<std::string, std::function<std::shared_ptr<RPG::IComponent>(nlohmann::json, std::shared_ptr<RPG::GameObject> go)>> pair) {
	jsonTypesToComponent.insert(pair);
}

void Serializer::LoadDefaultPropertyTypes() {
	LoadDefaultSavePropertyTypes();
	LoadDefaultLoadComponentTypes();
}

void Serializer::LoadDefaultSavePropertyTypes() {
	AddPropertySave({"std::string", [](std::shared_ptr<RPG::Property> property) -> nlohmann::json {
		std::any prop = property->GetProperty();
		std::string v = std::any_cast<std::string>(prop);

		json obj = json::object();

		obj["Value"] = v;

		return obj;
	}});

	AddPropertySave({"int", [](std::shared_ptr<RPG::Property> property) -> nlohmann::json {
		std::any prop = property->GetProperty();
		int v = std::any_cast<int>(prop);

		json obj = json::object();

		obj["Value"] = v;

		return obj;
	}});

	AddPropertySave({"float", [](std::shared_ptr<RPG::Property> property) -> nlohmann::json {
		std::any prop = property->GetProperty();
		float v = std::any_cast<float>(prop);

		json obj = json::object();

		obj["Value"] = v;

		return obj;
	}});

	AddPropertySave({"bool", [](std::shared_ptr<RPG::Property> property) -> nlohmann::json {
		std::any prop = property->GetProperty();
		bool v = std::any_cast<bool>(prop);

		json obj = json::object();

		obj["Value"] = v;

		return obj;
	}});

	AddPropertySave({"glm::vec2", [](std::shared_ptr<RPG::Property> property) -> nlohmann::json {
		std::any prop = property->GetProperty();
		glm::vec2 v = std::any_cast<glm::vec2>(prop);

		json obj = json::object();

		obj["Value"]["x"] = v.x;
		obj["Value"]["y"] = v.y;

		return obj;
	}});

	AddPropertySave({"glm::vec3", [](std::shared_ptr<RPG::Property> property) -> nlohmann::json {
		std::any prop = property->GetProperty();
		glm::vec3 v = std::any_cast<glm::vec3>(prop);

		json obj = json::object();

		obj["Value"]["x"] = v.x;
		obj["Value"]["y"] = v.y;
		obj["Value"]["z"] = v.z;

		return obj;
	}});

	AddPropertySave({"RPG::CameraType", [](std::shared_ptr<RPG::Property> property) -> nlohmann::json {
		std::any prop = property->GetProperty();
		RPG::CameraType v = std::any_cast<RPG::CameraType>(prop);

		json obj = json::object();

		obj["Value"] = static_cast<int>(v);

		return obj;
	}});

	AddPropertySave({"RPG::Assets::StaticMesh", [](std::shared_ptr<RPG::Property> property) -> nlohmann::json {
		std::any prop = property->GetProperty();
		RPG::Assets::StaticMesh v = std::any_cast<RPG::Assets::StaticMesh>(prop);

		json obj = json::object();

		obj["Value"] = static_cast<int>(v);

		return obj;
	}});

	AddPropertySave({"RPG::Assets::Texture", [](std::shared_ptr<RPG::Property> property) -> nlohmann::json {
		std::any prop = property->GetProperty();
		RPG::Assets::Texture v = std::any_cast<RPG::Assets::Texture>(prop);

		json obj = json::object();

		obj["Value"] = static_cast<int>(v);

		return obj;
	}});
}

void Serializer::LoadDefaultLoadComponentTypes() {
	AddComponentLoad({"TransformComponent", [](nlohmann::json j, std::shared_ptr<RPG::GameObject> go) -> std::shared_ptr<RPG::IComponent> {
		std::shared_ptr<RPG::TransformComponent> component = std::make_unique<RPG::TransformComponent>(j["Guid"].get<std::string>());

		go->GetTransform()->SetPosition( {
			j["Properties"][0]["Value"]["x"].get<float>(),
			j["Properties"][0]["Value"]["y"].get<float>(),
			j["Properties"][0]["Value"]["z"].get<float>()
		});
		go->GetTransform()->SetRotation({
			j["Properties"][1]["Value"]["x"].get<float>(),
			j["Properties"][1]["Value"]["y"].get<float>(),
			j["Properties"][1]["Value"]["z"].get<float>()
		});
		go->GetTransform()->SetScale({
			j["Properties"][2]["Value"]["x"].get<float>(),
			j["Properties"][2]["Value"]["y"].get<float>(),
			j["Properties"][2]["Value"]["z"].get<float>()
		});

		return component; //This wont be added to the gameobject anyways because we dont allow multiples
	}});

	AddComponentLoad({"MeshComponent", [](nlohmann::json j, std::shared_ptr<RPG::GameObject> go) -> std::shared_ptr<RPG::IComponent> {
		RPG::Assets::StaticMesh mesh = static_cast<RPG::Assets::StaticMesh>(j["Properties"][0]["Value"].get<int>());
		RPG::Assets::Texture texture = static_cast<RPG::Assets::Texture>(j["Properties"][1]["Value"].get<int>());
		return std::make_unique<RPG::MeshComponent>(mesh, texture, j["Guid"].get<std::string>());
	}});

	AddComponentLoad({"SpriteComponent", [](nlohmann::json j, std::shared_ptr<RPG::GameObject> go) -> std::shared_ptr<RPG::IComponent> {
		RPG::Assets::Texture texture = static_cast<RPG::Assets::Texture>(j["Properties"][0]["Value"].get<int>());
		return std::make_unique<RPG::SpriteComponent>(texture, j["Guid"].get<std::string>());
	}});

	AddComponentLoad({"LuaScriptComponent", [](nlohmann::json j, std::shared_ptr<RPG::GameObject> go) -> std::shared_ptr<RPG::IComponent> {
		return std::make_unique<RPG::LuaScriptComponent>(j["Properties"][0]["Value"].get<std::string>(), go, j["Guid"].get<std::string>());
	}});

	AddComponentLoad({"CameraComponent", [](nlohmann::json j, std::shared_ptr<RPG::GameObject> go) -> std::shared_ptr<RPG::IComponent> {
		//TODO: Fix this to camera properties
		auto size = RPG::ApplicationStats::GetInstance().GetWindowSize();
		auto component = std::make_unique<RPG::CameraComponent>(size.x, size.y, go->GetTransform(), j["Guid"].get<std::string>());
		component->SetDistance(j["Properties"][1]["Value"].get<float>());
		component->SetIsMainCamera(j["Properties"][2]["Value"].get<bool>());

		return component;
	}});
}