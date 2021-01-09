//
// Created by Alex on 12/30/2020.
//

#include "LuaScriptComponent.hpp"
#include "../LuaWrapper.hpp"
#include "../Log.hpp"
#include "../Assets.hpp"
#include "../Guid.hpp"

using RPG::LuaScriptComponent;

struct LuaScriptComponent::Internal {
	std::string guid;
	std::shared_ptr<RPG::Property> path;
	lua_State* L;
	bool isRunnable = false;

	Internal(std::string path, std::string guid)  : guid(guid),
								  path(std::make_unique<RPG::Property>(path, "Path", "std::string")),
								  L(luaL_newstate()) {}

	~Internal() {
		lua_close(L);
	}

	void Awake() {
		auto Log = [](lua_State* L) -> int {
			if (lua_gettop(L) != 1) return -1;
			const char* value = lua_tostring(L, -1);
			RPG::Log("Lua", value);
			return 0;
		};

		luaL_openlibs(L);
		lua_pushcfunction(L, Log);
		lua_setglobal(L, "Log");

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
		lua_pcall(L, 0, 0, 0);
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
};

LuaScriptComponent::LuaScriptComponent(std::string path, std::string guid) : internal(MakeInternalPointer<Internal>(path, guid)) {}

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