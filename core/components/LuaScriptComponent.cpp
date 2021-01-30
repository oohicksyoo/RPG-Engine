//
// Created by Alex on 12/30/2020.
//

#include "LuaScriptComponent.hpp"
#include "../LuaWrapper.hpp"
#include "../Log.hpp"
#include "../Assets.hpp"
#include "../Guid.hpp"
#include "../GameObject.hpp"
#include "../SceneManager.hpp"
#include "../input/InputManager.hpp"
#include "MeshComponent.hpp"
#include "PhysicsComponent.hpp"

using RPG::LuaScriptComponent;

struct LuaScriptComponent::Internal {
	std::string guid;
	std::shared_ptr<RPG::Property> path;
	std::shared_ptr<RPG::GameObject> myGameObject;
	lua_State* L;
	bool isRunnable = false;

	Internal(std::string path, std::shared_ptr<RPG::GameObject> gameObject, std::string guid)  : guid(guid),
								  path(std::make_unique<RPG::Property>(path, "Path", "RPG::Resource::String", true, "Lua")),
								  myGameObject(gameObject),
								  L(luaL_newstate()) {}

	~Internal() {
		lua_close(L);
	}

	void Awake() {
		std::string luaScript = std::any_cast<std::string>(path->GetProperty());
		if (luaScript == "") return;

		luaL_openlibs(L);
		CreateBindingFunctions();

		int result = luaL_dostring(L, RPG::Assets::LoadTextFile(luaScript).c_str());
		if (result != LUA_OK) {
			RPG::Log("Lua", lua_tostring(L, -1));
		}

		isRunnable = true;
	}

	void Start() {
		if (!isRunnable) return;

		lua_getglobal(L, "Class");
		lua_getfield(L, -1, "OnStart");
		void* memory = lua_newuserdata(L, sizeof(std::shared_ptr<RPG::GameObject>));
		new(memory) std::shared_ptr<RPG::GameObject>(myGameObject);
		lua_pcall(L, 1, 0, 0);
	}

	void Update(const float &delta) {
		if (!isRunnable) return;

		lua_getglobal(L, "Class");
		lua_getfield(L, -1, "OnUpdate");
		lua_pushnumber(L, delta);
		lua_pcall(L, 1, 0, 0);

		lua_getglobal(L, "Class");
		lua_getfield(L, -1, "Resume");
		lua_pushnumber(L, delta);
		lua_pcall(L, 1, 0, 0);
	}

	void CreateBindingFunctions() {

		lua_pushcfunction(L, [](lua_State* L) -> int {
			if (lua_gettop(L) != 1) return -1;
			const char* value = lua_tostring(L, -1);
			RPG::Log("Lua", value);
			return 0;
		});
		lua_setglobal(L, "Log");

		/*//Creates a GameObject for the lua script
		lua_pushcfunction(L, [](lua_State* L) -> int {
			void* memory = lua_newuserdata(L, sizeof(std::shared_ptr<RPG::GameObject>));
			auto gameObject = std::make_shared<RPG::GameObject>();
			new(memory) std::shared_ptr<RPG::GameObject>(gameObject);

			gameObject->AddComponent(std::make_shared<RPG::MeshComponent>(RPG::Assets::StaticMesh::Crate, RPG::Assets::Texture::Crate));

			RPG::SceneManager::GetInstance().GetCurrentScene()->GetHierarchy()->Add(gameObject);

			return 1;
		});
		lua_setglobal(L, "CreateGameObject");*/

		//Get Component - std::shared_prt<RPG::IComponent>
		lua_pushcfunction(L, ([](lua_State* L) -> int {
			if (lua_gettop(L) != 2) return -1;
			auto gameObject = static_cast<std::shared_ptr<RPG::GameObject>*>(lua_touserdata(L, -2))->get();
			std::string componentName = lua_tostring(L, -1);

			if (componentName == "PhysicsComponent") {
				auto c = gameObject->GetComponent<std::shared_ptr<RPG::PhysicsComponent>, RPG::PhysicsComponent>("PhysicsComponent");
				if (c == nullptr) return 0;
				void* memory = lua_newuserdata(L, sizeof(std::shared_ptr<RPG::PhysicsComponent>));
				new(memory) std::shared_ptr<RPG::PhysicsComponent>(c);
				return 1;
			}

			return 0;
		}));
		lua_setglobal(L, "GetComponent");

		//SetPosition for Gameobject
		lua_pushcfunction(L, [](lua_State* L) -> int {
			if (lua_gettop(L) != 4) return -1;
			auto gameObject = static_cast<std::shared_ptr<RPG::GameObject>*>(lua_touserdata(L, -4));

			float posX = (float)lua_tonumber(L, -3);
			float posY = (float)lua_tonumber(L, -2);
			float posZ = (float)lua_tonumber(L, -1);
			gameObject->get()->GetTransform()->SetPosition({posX, posY, posZ});
			return 0;
		});
		lua_setglobal(L, "SetPosition");

		//Set rotation for Gameobject
		lua_pushcfunction(L, [](lua_State* L) -> int {
			if (lua_gettop(L) != 4) return -1;
			auto gameObject = static_cast<std::shared_ptr<RPG::GameObject>*>(lua_touserdata(L, -4));

			float rotX = (float)lua_tonumber(L, -3);
			float rotY = (float)lua_tonumber(L, -2);
			float rotZ = (float)lua_tonumber(L, -1);
			gameObject->get()->GetTransform()->SetRotation({rotX, rotY, rotZ});
			return 0;
		});
		lua_setglobal(L, "SetRotation");

		//Get GameObject Forward
		lua_pushcfunction(L, [](lua_State* L) -> int {
			if (lua_gettop(L) != 1) return -1;
			auto gameObject = static_cast<std::shared_ptr<RPG::GameObject>*>(lua_touserdata(L, -1));
			auto position = gameObject->get()->GetTransform()->GetForward();

			lua_newtable(L);
			lua_pushstring(L, "x");
			lua_pushnumber(L, position.x);
			lua_settable(L, -3);
			lua_pushstring(L, "y");
			lua_pushnumber(L, position.y);
			lua_settable(L, -3);
			lua_pushstring(L, "z");
			lua_pushnumber(L, position.z);
			lua_settable(L, -3);

			return 1;
		});
		lua_setglobal(L, "GetForward");

		//Get GameObject Up
		lua_pushcfunction(L, [](lua_State* L) -> int {
			if (lua_gettop(L) != 1) return -1;
			auto gameObject = static_cast<std::shared_ptr<RPG::GameObject>*>(lua_touserdata(L, -1));
			auto position = gameObject->get()->GetTransform()->GetUp();

			lua_newtable(L);
			lua_pushstring(L, "x");
			lua_pushnumber(L, position.x);
			lua_settable(L, -3);
			lua_pushstring(L, "y");
			lua_pushnumber(L, position.y);
			lua_settable(L, -3);
			lua_pushstring(L, "z");
			lua_pushnumber(L, position.z);
			lua_settable(L, -3);

			return 1;
		});
		lua_setglobal(L, "GetUp");

		//Get GameObject Right
		lua_pushcfunction(L, [](lua_State* L) -> int {
			if (lua_gettop(L) != 1) return -1;
			auto gameObject = static_cast<std::shared_ptr<RPG::GameObject>*>(lua_touserdata(L, -1));
			auto position = gameObject->get()->GetTransform()->GetRight();

			lua_newtable(L);
			lua_pushstring(L, "x");
			lua_pushnumber(L, position.x);
			lua_settable(L, -3);
			lua_pushstring(L, "y");
			lua_pushnumber(L, position.y);
			lua_settable(L, -3);
			lua_pushstring(L, "z");
			lua_pushnumber(L, position.z);
			lua_settable(L, -3);

			return 1;
		});
		lua_setglobal(L, "GetRight");

		//Get GameObject Position
		lua_pushcfunction(L, [](lua_State* L) -> int {
			if (lua_gettop(L) != 1) return -1;
			auto gameObject = static_cast<std::shared_ptr<RPG::GameObject>*>(lua_touserdata(L, -1));
			auto position = gameObject->get()->GetTransform()->GetPosition();

			lua_newtable(L);
			lua_pushstring(L, "x");
			lua_pushnumber(L, position.x);
			lua_settable(L, -3);
			lua_pushstring(L, "y");
			lua_pushnumber(L, position.y);
			lua_settable(L, -3);
			lua_pushstring(L, "z");
			lua_pushnumber(L, position.z);
			lua_settable(L, -3);

			return 1;
		});
		lua_setglobal(L, "GetPosition");

		//Get GameObject Rotation
		lua_pushcfunction(L, [](lua_State* L) -> int {
			if (lua_gettop(L) != 1) return -1;
			auto gameObject = static_cast<std::shared_ptr<RPG::GameObject>*>(lua_touserdata(L, -1));
			auto rotation = gameObject->get()->GetTransform()->GetRotation();

			lua_newtable(L);
			lua_pushstring(L, "x");
			lua_pushnumber(L, rotation.x);
			lua_settable(L, -3);
			lua_pushstring(L, "y");
			lua_pushnumber(L, rotation.y);
			lua_settable(L, -3);
			lua_pushstring(L, "z");
			lua_pushnumber(L, rotation.z);
			lua_settable(L, -3);

			return 1;
		});
		lua_setglobal(L, "GetRotation");

		//Physics Component
		//Get Physics Velocity
		lua_pushcfunction(L, [](lua_State* L) -> int {
			if (lua_gettop(L) != 1) return -1;
			auto pc = static_cast<std::shared_ptr<RPG::PhysicsComponent>*>(lua_touserdata(L, -1))->get();
			auto velocity = pc->GetVelocity();

			lua_newtable(L);
			lua_pushstring(L, "x");
			lua_pushnumber(L, velocity.x);
			lua_settable(L, -3);
			lua_pushstring(L, "y");
			lua_pushnumber(L, velocity.y);
			lua_settable(L, -3);

			return 1;
		});
		lua_setglobal(L, "PhysicsGetVelocity");

		//Move GameObject to location
		lua_pushcfunction(L, [](lua_State* L) -> int {
			if (lua_gettop(L) != 3) return -1;
			auto pc = static_cast<std::shared_ptr<RPG::PhysicsComponent>*>(lua_touserdata(L, -3))->get();

			float posX = (float)lua_tonumber(L, -2);
			float posY = (float)lua_tonumber(L, -1);
			pc->SetVelocity({posX, posY});
			return 0;
		});
		lua_setglobal(L, "PhysicsSetVelocity");

		//Math Lerp
		lua_pushcfunction(L, [](lua_State* L) -> int {
			if (lua_gettop(L) != 3) return -1;
			float a = (float)lua_tonumber(L, -3);
			float b = (float)lua_tonumber(L, -2);
			float t = (float)lua_tonumber(L, -1);

			lua_pushnumber(L, a + (b - a) * t);

			return 1;
		});
		lua_setglobal(L, "Lerp");

		lua_pushcfunction(L, [](lua_State* L) -> int {
			if (lua_gettop(L) != 2) return -1;

			float x = (float)lua_tonumber(L, -2);
			float y = (float)lua_tonumber(L, -1);

			lua_newtable(L);
			lua_pushstring(L, "x");
			lua_pushnumber(L, x);
			lua_settable(L, -3);
			lua_pushstring(L, "y");
			lua_pushnumber(L, y);
			lua_settable(L, -3);

			return 1;
		});
		lua_setglobal(L, "Vector2");

		lua_pushcfunction(L, [](lua_State* L) -> int {
			if (lua_gettop(L) != 3) return -1;

			float x = (float)lua_tonumber(L, -3);
			float y = (float)lua_tonumber(L, -2);
			float z = (float)lua_tonumber(L, -1);

			lua_newtable(L);
			lua_pushstring(L, "x");
			lua_pushnumber(L, x);
			lua_settable(L, -3);
			lua_pushstring(L, "y");
			lua_pushnumber(L, y);
			lua_settable(L, -3);
			lua_pushstring(L, "z");
			lua_pushnumber(L, z);
			lua_settable(L, -3);

			return 1;
		});
		lua_setglobal(L, "Vector3");


		//Input
		lua_pushcfunction(L, [](lua_State* L) -> int {
			if (lua_gettop(L) != 2) return -1;
			int value = lua_tointeger(L, -1);
			int controllerID = lua_tointeger(L, -2);
			lua_pushboolean(L, RPG::InputManager::GetInstance().IsControllerButtonDown(controllerID, (RPG::Input::ControllerButton)value));
			return 1;
		});
		lua_setglobal(L, "IsControllerButtonDown");

		lua_pushcfunction(L, [](lua_State* L) -> int {
			if (lua_gettop(L) != 2) return -1;
			int value = lua_tointeger(L, -1);
			int controllerID = lua_tointeger(L, -2);
			lua_pushboolean(L, RPG::InputManager::GetInstance().IsControllerButtonPressed(controllerID, (RPG::Input::ControllerButton)value));
			return 1;
		});
		lua_setglobal(L, "IsControllerButtonPressed");

		lua_pushcfunction(L, [](lua_State* L) -> int {
			if (lua_gettop(L) != 2) return -1;
			int value = lua_tointeger(L, -1);
			int controllerID = lua_tointeger(L, -2);
			lua_pushboolean(L, RPG::InputManager::GetInstance().IsControllerButtonReleased(controllerID, (RPG::Input::ControllerButton)value));
			return 1;
		});
		lua_setglobal(L, "IsControllerButtonReleased");

		lua_pushcfunction(L, [](lua_State* L) -> int {
			if (lua_gettop(L) != 2) return -1;
			int value = lua_tointeger(L, -1);
			int controllerID = lua_tointeger(L, -2);
			lua_pushnumber(L, RPG::InputManager::GetInstance().GetControllerAxis(controllerID, (RPG::Input::ControllerAxis)value));
			return 1;
		});
		lua_setglobal(L, "GetControllerAxis");

		lua_pushcfunction(L, [](lua_State* L) -> int {
			if (lua_gettop(L) != 2) return -1;
			int value = lua_tointeger(L, -1);
			int controllerID = lua_tointeger(L, -2);
			lua_pushnumber(L, RPG::InputManager::GetInstance().GetControllerAxisHeldTime(controllerID, (RPG::Input::ControllerAxis)value));
			return 1;
		});
		lua_setglobal(L, "GetControllerAxisHeldTime");
	}
};

LuaScriptComponent::LuaScriptComponent(std::string path, std::shared_ptr<RPG::GameObject> gameObject, std::string guid) : internal(MakeInternalPointer<Internal>(path, gameObject, guid)) {}

void LuaScriptComponent::Awake() {
	internal->Awake();
}

void LuaScriptComponent::Start() {
	internal->Start();
}

void LuaScriptComponent::Update(const float &delta) {
	internal->Update(delta);
}

std::string LuaScriptComponent::Guid() {
	return internal->guid;
}

std::vector<std::shared_ptr<RPG::Property>> LuaScriptComponent::GetProperties() {
	std::vector<std::shared_ptr<RPG::Property>> list = {};
	list.push_back(internal->path);

	return list;
}