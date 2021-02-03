//
// Created by Alex on 12/30/2020.
//

#include "LuaScriptComponent.hpp"
#include "../LuaWrapper.hpp"
#include "../Assets.hpp"
#include "../SceneManager.hpp"
#include "../input/InputManager.hpp"
#include "../../application/ApplicationStats.hpp"
#include "../PhysicsSystem.hpp"
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

		//Add Component - gameobject, string, optional
		lua_pushcfunction(L, ([](lua_State* L) -> int {
			int stackSize = lua_gettop(L);
			if (stackSize < 2) return -1;
			auto gameObject = static_cast<std::shared_ptr<RPG::GameObject>*>(lua_touserdata(L, -stackSize))->get();
			std::string componentName = lua_tostring(L, -stackSize + 1);

			if (componentName == "MeshComponent") {
				auto mc = std::make_shared<RPG::MeshComponent>(RPG::MeshComponent(lua_tostring(L, -stackSize + 2), lua_tostring(L, -stackSize + 3)));
				auto c = gameObject->AddComponent(mc);
				if (c == nullptr) return 0;
				void* memory = lua_newuserdata(L, sizeof(std::shared_ptr<RPG::MeshComponent>));
				new(memory) std::shared_ptr<RPG::MeshComponent>(mc);
				return 1;
			}

			if (componentName == "LuaScriptComponent") {
				auto ls = std::make_shared<RPG::LuaScriptComponent>(RPG::LuaScriptComponent(lua_tostring(L, -stackSize + 2),
																							static_cast<std::shared_ptr<RPG::GameObject>>(gameObject)));
				auto c = gameObject->AddComponent(ls);
				if (c == nullptr) return 0;
				//Safe to start scripting calls
				ls->Awake();
				ls->Start();

				void* memory = lua_newuserdata(L, sizeof(std::shared_ptr<RPG::LuaScriptComponent>));
				new(memory) std::shared_ptr<RPG::LuaScriptComponent>(ls);
				return 1;
			}

			//TODO: Only handles Circle shape for now
			if (componentName == "PhysicsComponent") {
				std::shared_ptr<RPG::PhysicsComponent> pc = std::make_shared<RPG::PhysicsComponent>(RPG::PhysicsComponent(gameObject->GetTransform(), [gameObject]() -> std::vector<std::shared_ptr<RPG::IComponent>> {
					return gameObject->GetLuaScripts();
				}));
				pc->SetIsStatic(lua_toboolean(L, -stackSize + 2));
				pc->SetIsTrigger(lua_toboolean(L, -stackSize + 3));
				pc->SetMass((float)lua_tonumber(L, -stackSize + 4));
				pc->SetPhysicsShape(RPG::PhysicsShape::Circle);
				pc->SetDiameter((float)lua_tonumber(L, -stackSize + 5));
				RPG::PhysicsSystem::GetInstance().RegisterPhysicsComponent(pc);

				auto c = gameObject->AddComponent(pc);
				if (c == nullptr) return 0;
				void* memory = lua_newuserdata(L, sizeof(std::shared_ptr<RPG::PhysicsComponent>));
				new(memory) std::shared_ptr<RPG::PhysicsComponent>(pc);
				return 1;
			}

			if (componentName == "CameraComponent") {
				auto size = RPG::ApplicationStats::GetInstance().GetWindowSize();
				std::shared_ptr<RPG::CameraComponent> cc = std::make_shared<RPG::CameraComponent>(RPG::CameraComponent(size.x, size.y, gameObject->GetTransform()));
				cc->SetDistance((float)lua_tonumber(L, -stackSize + 2));
				cc->SetIsMainCamera(lua_toboolean(L, -stackSize + 3));
				cc->SetYaw((float)lua_tonumber(L, -stackSize + 4));
				cc->SetPitch((float)lua_tonumber(L, -stackSize + 5));

				auto c = gameObject->AddComponent(cc);
				if (c == nullptr) return 0;
				void* memory = lua_newuserdata(L, sizeof(std::shared_ptr<RPG::CameraComponent>));
				new(memory) std::shared_ptr<RPG::CameraComponent>(cc);
				return 1;
			}

			return 0;
		}));
		lua_setglobal(L, "AddComponent");

		//Get Child of a GameObject
		lua_pushcfunction(L, [](lua_State* L) -> int {
			int stackSize = lua_gettop(L);
			if (stackSize != 2) return -1;
			auto gameObject = static_cast<std::shared_ptr<RPG::GameObject>*>(lua_touserdata(L, -stackSize))->get();

			if (gameObject->HasChildren()) {
				auto child = gameObject->GetChildren()[(int)lua_tonumber(L, -stackSize + 1)];
				void* memory = lua_newuserdata(L, sizeof(std::shared_ptr<RPG::GameObject>));
				new(memory) std::shared_ptr<RPG::GameObject>(child);
				return 1;
			}
			return 0;
		});
		lua_setglobal(L, "GetChild");

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

		//Set Scale for Gameobject
		lua_pushcfunction(L, [](lua_State* L) -> int {
			if (lua_gettop(L) != 4) return -1;
			auto gameObject = static_cast<std::shared_ptr<RPG::GameObject>*>(lua_touserdata(L, -4));

			float sX = (float)lua_tonumber(L, -3);
			float sY = (float)lua_tonumber(L, -2);
			float sZ = (float)lua_tonumber(L, -1);
			gameObject->get()->GetTransform()->SetScale({sX, sY, sZ});
			return 0;
		});
		lua_setglobal(L, "SetScale");

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

		//Get GameObject Scale
		lua_pushcfunction(L, [](lua_State* L) -> int {
			if (lua_gettop(L) != 1) return -1;
			auto gameObject = static_cast<std::shared_ptr<RPG::GameObject>*>(lua_touserdata(L, -1));
			auto scale = gameObject->get()->GetTransform()->GetScale();

			lua_newtable(L);
			lua_pushstring(L, "x");
			lua_pushnumber(L, scale.x);
			lua_settable(L, -3);
			lua_pushstring(L, "y");
			lua_pushnumber(L, scale.y);
			lua_settable(L, -3);
			lua_pushstring(L, "z");
			lua_pushnumber(L, scale.z);
			lua_settable(L, -3);

			return 1;
		});
		lua_setglobal(L, "GetScale");

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
			if (lua_gettop(L) != 1) return -1;
			int value = lua_tointeger(L, -1);
			lua_pushboolean(L, RPG::InputManager::GetInstance().IsKeyDown((RPG::Input::Key)value));
			return 1;
		});
		lua_setglobal(L, "IsKeyDown");

		lua_pushcfunction(L, [](lua_State* L) -> int {
			if (lua_gettop(L) != 1) return -1;
			int value = lua_tointeger(L, -1);
			lua_pushboolean(L, RPG::InputManager::GetInstance().IsKeyPressed((RPG::Input::Key)value));
			return 1;
		});
		lua_setglobal(L, "IsKeyPressed");

		lua_pushcfunction(L, [](lua_State* L) -> int {
			if (lua_gettop(L) != 1) return -1;
			int value = lua_tointeger(L, -1);
			lua_pushboolean(L, RPG::InputManager::GetInstance().IsKeyReleased((RPG::Input::Key)value));
			return 1;
		});
		lua_setglobal(L, "IsKeyReleased");

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

		//Jam Game Special Bindings
		//Creates a GameObject: String Name, Optional Parent
		lua_pushcfunction(L, [](lua_State* L) -> int {
			int stackSize = lua_gettop(L);
			if (stackSize < 1) return -1;
			std::string name = lua_tostring(L, -stackSize);
			if (stackSize == 2) {
				auto parent = static_cast<std::shared_ptr<RPG::GameObject>*>(lua_touserdata(L, -stackSize + 1))->get();
				//Needs to happen after otherwise we grabbing it from the stack again
				void* memory = lua_newuserdata(L, sizeof(std::shared_ptr<RPG::GameObject>));
				auto gameObject = std::make_shared<RPG::GameObject>(name);
				new(memory) std::shared_ptr<RPG::GameObject>(gameObject);
				gameObject->SetParent(gameObject, static_cast<std::shared_ptr<RPG::GameObject>>(parent));
			} else {
				void* memory = lua_newuserdata(L, sizeof(std::shared_ptr<RPG::GameObject>));
				auto gameObject = std::make_shared<RPG::GameObject>(name);
				new(memory) std::shared_ptr<RPG::GameObject>(gameObject);

				RPG::SceneManager::GetInstance().GetCurrentScene()->GetHierarchy()->Add(gameObject);
			}

			return 1;
		});
		lua_setglobal(L, "CreateGameObject");
	}

	void OnTriggerEnter() {
		lua_getglobal(L, "Class");
		lua_getfield(L, -1, "OnTriggerEnter");
		lua_pcall(L, 0, 0, 0);
	}

	void OnTriggerStay(float delta) {
		lua_getglobal(L, "Class");
		lua_getfield(L, -1, "OnTriggerStay");
		lua_pushnumber(L, delta);
		lua_pcall(L, 1, 0, 0);
	}

	void OnTriggerExit() {
		lua_getglobal(L, "Class");
		lua_getfield(L, -1, "OnTriggerExit");
		lua_pcall(L, 0, 0, 0);
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

void LuaScriptComponent::OnTriggerEnter() {
	internal->OnTriggerEnter();
}

void LuaScriptComponent::OnTriggerStay(float delta) {
	internal->OnTriggerStay(delta);
}

void LuaScriptComponent::OnTriggerExit() {
	internal->OnTriggerExit();
}

std::vector<std::shared_ptr<RPG::Property>> LuaScriptComponent::GetProperties() {
	std::vector<std::shared_ptr<RPG::Property>> list = {};
	list.push_back(internal->path);

	return list;
}