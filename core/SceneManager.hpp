//
// Created by Alex on 1/8/2021.
//

#pragma once
#include "Singleton.hpp"
#include "IScene.hpp"
#include <string>

namespace RPG {
	struct SceneManager : public Singleton<SceneManager> {
		friend struct Singleton<SceneManager>;

		public:
			SceneManager();
			std::shared_ptr<RPG::IScene> GetCurrentScene();
			std::shared_ptr<RPG::IScene> CreateNewScene();
			std::shared_ptr<RPG::IScene> LoadScene(std::string path);
			void SaveCurrentScene(std::string path);
			void StoreCurrentScene();
			void ReloadCurrentScene();

		private:
			std::shared_ptr<RPG::IScene> currentScene;
			std::string currentSavedScene;


	};
}


