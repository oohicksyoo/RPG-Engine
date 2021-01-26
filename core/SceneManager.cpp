//
// Created by Alex on 1/8/2021.
//

#include "SceneManager.hpp"
#include "Serializer.hpp"
#include "../application/ApplicationStats.hpp"
#include "IScene.hpp"
#include "Scene.hpp"
#include "GLMWrapper.hpp"
#include "Assets.hpp"

using RPG::SceneManager;

SceneManager::SceneManager() {
	currentSavedScene = "";
	currentScenePath = "";
	LoadScene("assets/scenes/demo.scene"); //TODO: Move this to something project specific
}

std::shared_ptr<RPG::IScene> SceneManager::GetCurrentScene() {
	return (currentScene != nullptr) ? currentScene : CreateNewScene();
}

std::shared_ptr<RPG::IScene> SceneManager::CreateNewScene() {
	if (currentScene != nullptr) {
		//TODO: Some sort of shutdown I presume
	}
	glm::vec2 size = RPG::ApplicationStats::GetInstance().GetWindowSize();
	currentScene = std::make_unique<RPG::Scene>(RPG::Scene({static_cast<uint32_t>(size.x), static_cast<uint32_t>(size.y)}));
	currentScenePath = "";
	return currentScene;
}

std::shared_ptr<RPG::IScene> SceneManager::LoadScene(std::string path) {
	if (currentScene != nullptr) {
		//TODO: Some sort of shutdown I presume
	}
	glm::vec2 size = RPG::ApplicationStats::GetInstance().GetWindowSize();
	currentScene = RPG::Serializer::GetInstance().LoadScene({static_cast<uint32_t>(size.x), static_cast<uint32_t>(size.y)}, path);
	currentScenePath = path;
	return currentScene;
}

void SceneManager::SaveCurrentScene(std::string path) {
	if (currentScene != nullptr) {
		RPG::Serializer::GetInstance().SaveScene(currentScene, path);
	}
}

void SceneManager::SaveCurrentScene() {
	if (currentScenePath != "") {
		RPG::Serializer::GetInstance().SaveScene(currentScene, currentScenePath);
	}
}

void SceneManager::StoreCurrentScene() {
	if (currentScene != nullptr) {
		currentSavedScene = RPG::Serializer::GetInstance().SaveScene(currentScene, "assets/scenes/__Temp__");
		RPG::Assets::RemoveFile("assets/scenes/__Temp__");
	}
}

void SceneManager::ReloadCurrentScene() {
	if (currentSavedScene != "") {
		glm::vec2 size = RPG::ApplicationStats::GetInstance().GetWindowSize();
		currentScene = RPG::Serializer::GetInstance().LoadSceneData({static_cast<uint32_t>(size.x), static_cast<uint32_t>(size.y)}, currentSavedScene);
		currentSavedScene = "";
	}
}

bool SceneManager::HasCurrentScenePath() {
	return currentScenePath != "";
}