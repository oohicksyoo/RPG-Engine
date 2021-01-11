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
			std::shared_ptr<RPG::IScene> GetCurrentScene();
			std::shared_ptr<RPG::IScene> CreateNewScene();
			std::shared_ptr<RPG::IScene> LoadScene(std::string path);
			void SaveCurrentScene(std::string path);

		private:
			std::shared_ptr<RPG::IScene> currentScene;


	};
}

