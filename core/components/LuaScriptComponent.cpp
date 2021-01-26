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

using RPG::LuaScriptComponent;

struct LuaScriptComponent::Internal {
	std::string guid;
	std::shared_ptr<RPG::Property> path;
	std::shared_ptr<RPG::GameObject> myGameObject;
	lua_State* L;
	bool isRunnable = false;

	Internal(std::string path, std::shared_ptr<RPG::GameObject> gameObject, std::string guid)  : guid(guid),
								  path(std::make_unique<RPG::Property>(path, "Path", "std::string")),
								  myGameObject(gameObject),
								  L(luaL_newstate()) {}

	~Internal() {
		lua_close(L);
	}

	void Awake() {

		luaL_openlibs(L);
		CreateBindingFunctions();

		std::string luaScript = std::any_cast<std::string>(path->GetProperty());
		int result = luaL_dostring(L, RPG::Assets::LoadTextFile(luaScript).c_str());
		if (result != LUA_OK) {
			RPG::Log("Lua", lua_tostring(L, -1));
		}

		isRunnable = true;
	}

	void Start() {
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

		//Move GameObject to location
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