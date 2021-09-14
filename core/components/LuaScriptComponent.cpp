//
// Created by Alex on 12/30/2020.
//

#include "LuaScriptComponent.hpp"
#include "../Assets.hpp"
#include "../SceneManager.hpp"
#include "../input/InputManager.hpp"
#include "../../application/ApplicationStats.hpp"
#include "../PhysicsSystem.hpp"
#include "MeshComponent.hpp"
#include "PhysicsComponent.hpp"
#include "../SDLWrapper.hpp"
#include "../Log.hpp"
#include <filesystem>

using RPG::LuaScriptComponent;

struct LuaScriptComponent::Internal {
	std::string guid;
	std::shared_ptr<RPG::Property> path;
	std::shared_ptr<RPG::GameObject> myGameObject;
	lua_State* L;
	bool isRunnable = false;
	std::string luaScriptName;

	Internal(std::string path, std::shared_ptr<RPG::GameObject> gameObject, std::string guid)  : guid(guid),
								  path(std::make_unique<RPG::Property>(path, "Path", "RPG::Resource::String", true, "Lua")),
								  myGameObject(gameObject),
								  L(luaL_newstate()) {
	    RPG::LuaScriptComponent::mappedComponents.insert(std::make_pair(guid,std::vector<std::string>()));
	}

	~Internal() {
		lua_close(L);
	}

	bool LoadAndRun() {
        std::string luaScript = std::any_cast<std::string>(path->GetProperty());
        if (luaScript == "") return false;

        luaScriptName = std::filesystem::path(luaScript).stem().string();
        RPG::Log("Lua", luaScriptName);

        luaL_openlibs(L);

        //Add Application path to Lua to know where the data is
        if (SDL_GetBasePath() != NULL) {
            std::string basePath = SDL_GetBasePath();
            SetupLuaPath(basePath + "?.lua");
            SetupLuaPath(basePath + "assets/scripts/?.lua");
        }

        CreateBindingFunctions();

        //TODO: Statically mark we are the current executer by using our GUID
        RPG::LuaScriptComponent::mappedComponents[guid].clear();
        RPG::LuaScriptComponent::currentMappingGuid = guid;

        int result = luaL_dostring(L, RPG::Assets::LoadTextFile(luaScript).c_str());
        if (result != LUA_OK) {
            RPG::Log("Lua", lua_tostring(L, -1));
            return false;
        }

        RPG::Log("Lua", "Resetting currentMappingGuid");
        RPG::LuaScriptComponent::currentMappingGuid = "";
        return true;
	}


	//Loads the lua file grabbing its properties for use in the inspector
	void SyncProperties() {
        if (!LoadAndRun()) return;
	}

	void Awake() {
        if (!LoadAndRun()) return;

        lua_setglobal(L, "Class");
		lua_settop(L, 0);

        lua_getglobal(L, "Class");
        lua_getfield(L, -1, luaScriptName.c_str());
        lua_getfield(L, -1, "prototype");
        lua_getfield(L, -1, "____constructor");
        lua_pushvalue(L, -3);
        void* memory = lua_newuserdata(L, sizeof(std::shared_ptr<RPG::GameObject>));
        new(memory) std::shared_ptr<RPG::GameObject>(myGameObject);
        lua_pcall(L, 2, 0, 0);
        ClearStack();

        //Loop through list of serializables and set their values
        auto list = RPG::LuaScriptComponent::mappedComponents[guid];
        RPG::Log("Lua", std::to_string(list.size()));
        lua_getglobal(L, "Class");
        lua_getfield(L, -1, luaScriptName.c_str());
        for (auto str : list) {
            lua_getfield(L, -1, str.c_str());
        }
        LogStack(L);
        ClearStack();

		isRunnable = true;
	}

	void Start() {
		if (!isRunnable) return;

        lua_getglobal(L, "Class");
        lua_getfield(L, -1, luaScriptName.c_str());
        lua_getfield(L, -1, "prototype");
        lua_getfield(L, -1, "Start");
        lua_pushvalue(L, -3);
        lua_pcall(L, 1, 0, 0);
	}

	void Update(const float &delta) {
		if (!isRunnable) return;

        lua_getglobal(L, "Class");
        lua_getfield(L, -1, luaScriptName.c_str());
        lua_getfield(L, -1, "prototype");
        lua_getfield(L, -1, "Update");
        lua_pushvalue(L, -3);
        lua_pushnumber(L, delta);
        lua_pcall(L, 2, 0, 0);

        lua_getglobal(L, "Class");
        lua_getfield(L, -1, luaScriptName.c_str());
        lua_getfield(L, -1, "prototype");
        lua_getfield(L, -1, "Resume");
        lua_pushvalue(L, -3);
        lua_pushnumber(L, delta);
        lua_pcall(L, 2, 0, 0);
	}

	void SetupLuaPath(std::string path) {
		lua_getglobal(L, "package");
		lua_getfield(L, -1, "path"); // get field "path" from table at top of stack (-1)
		std::string cur_path = lua_tostring(L, -1); // grab path string from top of stack
		cur_path.append(";"); // do your path magic here
		cur_path.append(path.c_str());
		lua_pop(L, 1); // get rid of the string on the stack we just pushed on line 5
		lua_pushstring(L, cur_path.c_str()); // push the new one
		lua_setfield(L, -2, "path"); // set the field "path" in table at -2 with value at top of stack
		lua_pop(L, 1); // get rid of package table from top of stack
	}

    #pragma region Helpers

    static int LogStack(lua_State *L) {
        int topIndex = lua_gettop(L);
        RPG::Log("Lua", "-------Stack Start-------");
        RPG::Log("Lua", "Stack Size: " + std::to_string(topIndex));
        std::string v = "";
        for (int i = 1; i <= topIndex; i++) {
            std::string index = std::to_string(i);
            std::string rIndex = std::to_string(-topIndex + i - 1);
            std::string prefix = index + "(" + rIndex + "):";
            switch (lua_type(L, i)) {
                case LUA_TNUMBER:
                    RPG::Log("Lua", prefix + std::to_string(lua_tonumber(L, i)));
                    break;
                case LUA_TSTRING:
                    v = lua_tostring(L, i);
                    RPG::Log("Lua", prefix + v);
                    break;
                case LUA_TBOOLEAN:
                    v = lua_toboolean(L, i) ? "true" : "false";
                    RPG::Log("Lua", prefix + v);
                    break;
                case LUA_TNIL:
                    RPG::Log("Lua", prefix + "nil");
                    break;
                default:
                    RPG::Log("Lua", prefix + "Pointer");
                    break;
            }
        }
        RPG::Log("Lua", "-------Stack Done-------");
        return 0;
	}

    #pragma endregion

    #pragma region RPG

    static int Log(lua_State *L) {
        if (lua_gettop(L) != 2) return -1;
        const char* value = lua_tostring(L, -1);
        RPG::Log("Lua", value);
        return 0;
	}

    #pragma endregion

    #pragma region GameObject

    static int GetPosition(lua_State *L) {
        if (lua_gettop(L) != 2) return -1;
        auto gameObject = static_cast<std::shared_ptr<RPG::GameObject>*>(lua_touserdata(L, -1));
        auto rotation = gameObject->get()->GetTransform()->GetPosition();

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
    }

    static int SetPosition(lua_State *L) {
	    //RPG::Log("Lua", "SetPosition()");
        if (lua_gettop(L) != 3) return -1;
        //RPG::Log("Lua", "Enough params");
        auto gameObject = static_cast<std::shared_ptr<RPG::GameObject>*>(lua_touserdata(L, -2))->get();

        lua_getfield(L, -1, "x");
        lua_getfield(L, -2, "y");
        lua_getfield(L, -3, "z");

        float posX = (float)lua_tonumber(L, -3);
        float posY = (float)lua_tonumber(L, -2);
        float posZ = (float)lua_tonumber(L, -1);
        //RPG::Log("Lua", "Params: (" + std::to_string(posX) + ", "+ std::to_string(posY) + ", "+ std::to_string(posZ) + ")");
        gameObject->GetTransform()->SetPosition({posX, posY, posZ});
        return 0;
	}

	static int GetRotation(lua_State *L) {
        if (lua_gettop(L) != 2) return -1;
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
	}

	static int GetForward(lua_State *L) {
        if (lua_gettop(L) != 2) return -1;
        auto gameObject = static_cast<std::shared_ptr<RPG::GameObject>*>(lua_touserdata(L, -1));
        auto value = gameObject->get()->GetTransform()->GetForward();

        lua_newtable(L);
        lua_pushstring(L, "x");
        lua_pushnumber(L, value.x);
        lua_settable(L, -3);
        lua_pushstring(L, "y");
        lua_pushnumber(L, value.y);
        lua_settable(L, -3);
        lua_pushstring(L, "z");
        lua_pushnumber(L, value.z);
        lua_settable(L, -3);

        return 1;
	}

    #pragma endregion

    #pragma region RPGMath

    static int GetVector3(lua_State *L) {
        if (lua_gettop(L) != 1) return -1;

        lua_newtable(L);
        lua_pushstring(L, "x");
        lua_pushnumber(L, 5);
        lua_settable(L, -3);
        lua_pushstring(L, "y");
        lua_pushnumber(L, 6);
        lua_settable(L, -3);
        lua_pushstring(L, "z");
        lua_pushnumber(L, 7);
        lua_settable(L, -3);

        return 1;
    }

    #pragma endregion

    #pragma region Meta

    static int SetMeta(lua_State *L) {
        if (lua_gettop(L) != 4) return -1;
        std::string luaScript = lua_tostring(L, -3);
        std::string property = lua_tostring(L, -2);
        std::string typing = lua_tostring(L, -1);
        RPG::Log("Lua", "Setting Serialize Field: " + luaScript + " | " + property + " | " + typing);

        //TODO: Grab static marker and using that guid to map this field
        //TODO: Manually Call constructor
        //TODO: Set our value from the inspector?
        auto list = RPG::LuaScriptComponent::mappedComponents[RPG::LuaScriptComponent::currentMappingGuid];
        list.push_back(property);
        RPG::LuaScriptComponent::mappedComponents[RPG::LuaScriptComponent::currentMappingGuid] = list;

	    return 0;
	}

    #pragma endregion

    #pragma region Mouse

    static int MouseIsDown(lua_State *L) {
        if (lua_gettop(L) != 2) return -1;
        int value = lua_tonumber(L, -1);
        lua_pushboolean(L, RPG::InputManager::GetInstance().IsMouseButtonDown((RPG::Input::MouseButton)value));
	    return 1;
	}

    static int MouseIsPressed(lua_State *L) {
        if (lua_gettop(L) != 2) return -1;
        int value = lua_tonumber(L, -1);
        lua_pushboolean(L, RPG::InputManager::GetInstance().IsMouseButtonPressed((RPG::Input::MouseButton)value));
        return 1;
    }

    static int MouseIsReleased(lua_State *L) {
        if (lua_gettop(L) != 2) return -1;
        int value = lua_tonumber(L, -1);
        lua_pushboolean(L, RPG::InputManager::GetInstance().IsMouseButtonReleased((RPG::Input::MouseButton)value));
        return 1;
    }

    #pragma endregion

    #pragma region Keyboard

    static int KeyboardIsDown(lua_State *L) {
        if (lua_gettop(L) != 2) return -1;
        int value = lua_tonumber(L, -1);
        lua_pushboolean(L, RPG::InputManager::GetInstance().IsKeyDown((RPG::Input::Key)value));
        return 1;
    }

    static int KeyboardIsPressed(lua_State *L) {
        if (lua_gettop(L) != 2) return -1;
        int value = lua_tonumber(L, -1);
        lua_pushboolean(L, RPG::InputManager::GetInstance().IsKeyPressed((RPG::Input::Key)value));
        return 1;
    }

    static int KeyboardIsReleased(lua_State *L) {
        if (lua_gettop(L) != 2) return -1;
        int value = lua_tonumber(L, -1);
        lua_pushboolean(L, RPG::InputManager::GetInstance().IsKeyReleased((RPG::Input::Key)value));
        return 1;
    }

    #pragma endregion

	void CreateBindingFunctions() {
	    static const luaL_Reg functions[] = {{"Log", Log}, {"LogStack", LogStack}, {NULL, NULL}};
        static const luaL_Reg mathFuncs[] = {{"GetVector3", GetVector3}, {NULL, NULL}};
        static const luaL_Reg gameObjectFuncs[] = {{"GetPosition", GetPosition}, {"SetPosition", SetPosition}, {NULL, NULL}};
        static const luaL_Reg metaFuncs[] = {{"SetMeta", SetMeta}, {NULL, NULL}};
        static const luaL_Reg mouseFuncs[] = {{"IsDown", MouseIsDown}, {"IsPressed", MouseIsPressed}, {"IsReleased", MouseIsReleased}, {NULL, NULL}};
        static const luaL_Reg keyboardFuncs[] = {{"IsDown", KeyboardIsDown}, {"IsPressed", KeyboardIsPressed}, {"IsReleased", KeyboardIsReleased}, {NULL, NULL}};

        lua_newtable(L);
        luaL_setfuncs(L, functions, 0);
        lua_setglobal(L, "RPG");

        lua_newtable(L);
        luaL_setfuncs(L, mathFuncs, 0);
        lua_setglobal(L, "RPGMath");

        lua_newtable(L);
        luaL_setfuncs(L, gameObjectFuncs, 0);
        lua_setglobal(L, "GameObject");

        lua_newtable(L);
        luaL_setfuncs(L, metaFuncs, 0);
        lua_setglobal(L, "Meta");

        lua_newtable(L);
        luaL_setfuncs(L, mouseFuncs, 0);
        lua_setglobal(L, "Mouse");

        lua_newtable(L);
        luaL_setfuncs(L, keyboardFuncs, 0);
        lua_setglobal(L, "Keyboard");

        #pragma region Old Bindings
        /*lua_register(L, "Log", Log);
        lua_register(L, "GetPosition", GetPosition);
        //lua_register(L, "GetRotation", GetRotation);*/

	    //First value is nil for whatever fucking reason
        /*lua_pushcfunction(L, [](lua_State* L) -> int {
            if (lua_gettop(L) != 2) return -1;
            const char* value = lua_tostring(L, -1);
            RPG::Log("Lua", value);
            return 0;
        });
        lua_setglobal(L, "Log");*/

		/*lua_pushcfunction(L, [](lua_State* L) -> int {
			if (lua_gettop(L) != 1) return -1;
			const char* value = lua_tostring(L, -1);
			RPG::Log("Lua", value);
			return 0;
		});
		lua_setglobal(L, "Log");*/

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
		/*lua_pushcfunction(L, ([](lua_State* L) -> int {
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

			if (componentName == "MeshComponent") {
				auto c = gameObject->GetComponent<std::shared_ptr<RPG::MeshComponent>, RPG::MeshComponent>("MeshComponent");
				if (c == nullptr) return 0;
				void* memory = lua_newuserdata(L, sizeof(std::shared_ptr<RPG::MeshComponent>));
				new(memory) std::shared_ptr<RPG::MeshComponent>(c);
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

		//Remove Component - gameobject, string, optional
		lua_pushcfunction(L, ([](lua_State* L) -> int {
			int stackSize = lua_gettop(L);
			if (stackSize < 2) return -1;
			auto gameObject = static_cast<std::shared_ptr<RPG::GameObject>*>(lua_touserdata(L, -stackSize))->get();
			std::string componentName = lua_tostring(L, -stackSize + 1);

			auto c = gameObject->GetComponent<std::shared_ptr<RPG::MeshComponent>, RPG::MeshComponent>(componentName);

			gameObject->RemoveComponent(c->Guid());

			return 0;
		}));
		lua_setglobal(L, "RemoveComponent");

		lua_pushcfunction(L, ([](lua_State* L) -> int {
			int stackSize = lua_gettop(L);
			if (stackSize < 2) return -1;
			auto gameObject = static_cast<std::shared_ptr<RPG::GameObject>*>(lua_touserdata(L, -stackSize))->get();
			auto child = static_cast<std::shared_ptr<RPG::GameObject>*>(lua_touserdata(L, -stackSize))->get();

			gameObject->RemoveChild(static_cast<std::shared_ptr<RPG::GameObject>>(child));

			return 0;
		}));
		lua_setglobal(L, "RemoveChild");

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
			int stackSize = lua_gettop(L);
			if (stackSize != 6) return -1;
			float x1 = (float)lua_tonumber(L, -stackSize);
			float y1= (float)lua_tonumber(L, -stackSize + 1);
			float z1 = (float)lua_tonumber(L, -stackSize + 2);
			float x2 = (float)lua_tonumber(L, -stackSize + 3);
			float y2= (float)lua_tonumber(L, -stackSize + 4);
			float z2 = (float)lua_tonumber(L, -stackSize + 5);

			lua_pushnumber(L, glm::distance(glm::vec3{x1, y1, z1}, glm::vec3{x2, y2, z2}));

			return 1;
		});
		lua_setglobal(L, "Distance");

		lua_pushcfunction(L, [](lua_State* L) -> int {
			int stackSize = lua_gettop(L);
			if (stackSize != 4) return -1;
			float x = (float)lua_tonumber(L, -stackSize);
			float y= (float)lua_tonumber(L, -stackSize + 1);
			float z = (float)lua_tonumber(L, -stackSize + 2);
			float mult = (float)lua_tonumber(L, -stackSize + 3);

			lua_newtable(L);
			lua_pushstring(L, "x");
			lua_pushnumber(L, x * mult);
			lua_settable(L, -3);
			lua_pushstring(L, "y");
			lua_pushnumber(L, y * mult);
			lua_settable(L, -3);
			lua_pushstring(L, "z");
			lua_pushnumber(L, z * mult);
			lua_settable(L, -3);

			return 1;
		});
		lua_setglobal(L, "Vector3Multiply");

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

		//TODO: Somehow changing from Parent 1 to Parent 2 causes Parent 1 to be remove from memory even though it has other children and is in hierarchy
		lua_pushcfunction(L, [](lua_State* L) -> int {
			int stackSize = lua_gettop(L);
			if (stackSize < 1) return -1;

			auto gameObject = static_cast<std::shared_ptr<RPG::GameObject>*>(lua_touserdata(L, -stackSize))->get();

			if (stackSize == 2) {
				auto parent = static_cast<std::shared_ptr<RPG::GameObject>*>(lua_touserdata(L, -stackSize + 1))->get();
				if (!gameObject->HasParent()) {
					RPG::SceneManager::GetInstance().GetCurrentScene()->GetHierarchy()->Remove(static_cast<std::shared_ptr<RPG::GameObject>>(gameObject));
				} else {
					parent->RemoveChild(static_cast<std::shared_ptr<RPG::GameObject>>(gameObject));
				}

				gameObject->SetParent(static_cast<std::shared_ptr<RPG::GameObject>>(gameObject), static_cast<std::shared_ptr<RPG::GameObject>>(parent));
			} else {
				if (gameObject->HasParent()) {
					gameObject->SetParent(static_cast<std::shared_ptr<RPG::GameObject>>(gameObject), nullptr);
				}
				RPG::SceneManager::GetInstance().GetCurrentScene()->GetHierarchy()->Add(static_cast<std::shared_ptr<RPG::GameObject>>(gameObject));
			}
			return 0;
		});
		lua_setglobal(L, "SetParent");*/
        #pragma endregion
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

	void ClearStack() {
        lua_pop(L, -1);
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

void LuaScriptComponent::SyncProperties() {
    internal->SyncProperties();
}

std::vector<std::shared_ptr<RPG::Property>> LuaScriptComponent::GetProperties() {
	std::vector<std::shared_ptr<RPG::Property>> list = {};
	list.push_back(internal->path);

	return list;
}