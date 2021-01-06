//
// Created by Alex on 1/5/2021.
//

#include "Serializer.hpp"
#include "Scene.hpp"

#include "GameObject.hpp"
#include "Hierarchy.hpp"
#include "components/MeshComponent.hpp"
#include "components/SpriteComponent.hpp"

using RPG::Assets::Pipeline;
using RPG::Assets::StaticMesh;
using RPG::Assets::Texture;

void RPG::Serializer::SaveScene(std::unique_ptr<RPG::IScene> scene) {

}

std::unique_ptr<RPG::IScene> RPG::Serializer::LoadScene(const RPG::WindowSize& frameSize) {
	std::unique_ptr<RPG::IScene> scene = std::make_unique<RPG::Scene>(RPG::Scene(frameSize));

	auto hierarchy = scene->GetHierarchy();

	//Setup sample hierarchy for the engine
	{
		std::shared_ptr<RPG::GameObject> crateGameObject = std::make_unique<RPG::GameObject>(RPG::GameObject("Crate"));
		crateGameObject->GetTransform()->SetPosition({0.0f, 0.0f, -5.0f});
		crateGameObject->AddComponent(std::make_unique<RPG::MeshComponent>(RPG::MeshComponent(RPG::Assets::StaticMesh::Crate, RPG::Assets::Texture::Crate)));
		hierarchy->Add(crateGameObject);

		std::shared_ptr<RPG::GameObject> spriteGameObject = std::make_unique<RPG::GameObject>(RPG::GameObject("Empty"));
		spriteGameObject->GetTransform()->SetPosition({0.0f, 0.0f, 0.0f});
		spriteGameObject->GetTransform()->SetRotation({90, 0, 0});
		spriteGameObject->AddComponent(std::make_unique<RPG::SpriteComponent>(RPG::SpriteComponent(RPG::Assets::Texture::Sprite)));
		hierarchy->Add(spriteGameObject);
	}

	return scene;
}