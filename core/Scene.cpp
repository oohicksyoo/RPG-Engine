//
// Created by Alex on 1/5/2021.
//

#include "Scene.hpp"
#include "SDLWrapper.hpp"
#include "Log.hpp"
#include "Guid.hpp"
#include "components/CameraComponent.hpp"

using RPG::Scene;
using RPG::Assets::Pipeline;
using RPG::Assets::StaticMesh;
using RPG::Assets::Texture;

namespace {
	std::shared_ptr<RPG::GameObject> CreateSceneCamera(const RPG::WindowSize& size) {
		std::shared_ptr<RPG::GameObject> go = std::make_unique<RPG::GameObject>();
		go->AddComponent(std::make_unique<RPG::CameraComponent>(static_cast<float>(size.width), static_cast<float>(size.height)));
		return go;
	}
}

struct Scene::Internal {
	std::string guid;
	bool hasLoaded;
	const uint8_t* keyboardState;
	std::shared_ptr<RPG::Hierarchy> hierarchy;
	std::shared_ptr<RPG::GameObject> sceneCamera;
	std::shared_ptr<RPG::GameObject> gameCamera;

	Internal(const RPG::WindowSize& size, std::string guid) : guid(guid),
											hasLoaded(false),
											keyboardState(SDL_GetKeyboardState(nullptr)),
											hierarchy(std::make_unique<RPG::Hierarchy>(RPG::Hierarchy())),
											sceneCamera(::CreateSceneCamera(size)),
										    gameCamera(nullptr) {}

	RPG::AssetManifest GetAssetManifest() {
		return RPG::AssetManifest{
				{{Pipeline::Default, Pipeline::SceneLines}},
				{{StaticMesh::Quad, StaticMesh::Crate}},
				{{Texture::Crate, Texture::Sprite}}
		};
	}

	void Prepare() {
		hasLoaded = true;
	}

	void Awake() {
		for (auto gameObject : hierarchy->GetHierarchy()) {
			gameObject->Awake();
		}
	}

	void Start() {
		for (auto gameObject : hierarchy->GetHierarchy()) {
			gameObject->Start();
		}
	}

	void Update(const float& delta) {
		ProcessInput(delta);

		for (auto gameObject : hierarchy->GetHierarchy()) {
			gameObject->Update(delta);
		}
	}

	void Render(RPG::IRenderer& renderer) {
		renderer.Render(Pipeline::Default, hierarchy, GetSceneCameraMatrix());
	}

	void RenderToFrameBuffer(RPG::IRenderer& renderer, std::shared_ptr<RPG::FrameBuffer>frameBuffer, glm::vec3 clearColor, bool isGameCamera = true) {
		renderer.RenderToFrameBuffer(Pipeline::Default, hierarchy, frameBuffer, (isGameCamera) ? GetGameCameraMatrix() : GetSceneCameraMatrix(), clearColor);
	}

	void RenderLinesToFrameBuffer(RPG::IRenderer& renderer, std::shared_ptr<RPG::FrameBuffer> frameBuffer) {
		renderer.RenderLinesToFrameBuffer(Pipeline::SceneLines, frameBuffer, GetSceneCameraMatrix());
	}

	void OnWindowResized(const RPG::WindowSize& size) {
		sceneCamera = ::CreateSceneCamera(size);
	}

	std::shared_ptr<RPG::CameraComponent> GetSceneCameraComponent() {
		return sceneCamera->GetComponent<std::shared_ptr<RPG::CameraComponent>, RPG::CameraComponent>(std::make_unique<RPG::CameraComponent>(1, 1));
	}

	std::shared_ptr<RPG::CameraComponent> GetGameCameraComponent() {
		return gameCamera->GetComponent<std::shared_ptr<RPG::CameraComponent>, RPG::CameraComponent>(std::make_unique<RPG::CameraComponent>(1, 1));
	}

	glm::mat4 GetSceneCameraMatrix() {
		auto cameraComponent = GetSceneCameraComponent();
		return {cameraComponent->GetProjectionMatrix() * cameraComponent->GetViewMatrix(sceneCamera->GetTransform()->GetPosition())};
	}

	glm::mat4 GetGameCameraMatrix() {
		if (gameCamera == nullptr) {
			return GetSceneCameraMatrix();
		}

		auto cameraComponent = GetGameCameraComponent();
		return {cameraComponent->GetProjectionMatrix() * cameraComponent->GetViewMatrix(gameCamera->GetTransform()->GetPosition())};
	}

	void ProcessInput(const float& delta) {
		const uint8_t* keyboardState;
		keyboardState(SDL_GetKeyboardState(nullptr));

		//Update function
		if (keyboardState[SDL_SCANCODE_UP]) {
			//player.MoveForward(delta);
		}

		if (keyboardState[SDL_SCANCODE_DOWN]) {
			//player.MoveBackward(delta);
		}

		if (keyboardState[SDL_SCANCODE_A]) {
			//player.MoveUp(delta);
		}

		if (keyboardState[SDL_SCANCODE_Z]) {
			//player.MoveDown(delta);
		}

		if (keyboardState[SDL_SCANCODE_LEFT]) {
			//player.TurnLeft(delta);
		}

		if (keyboardState[SDL_SCANCODE_RIGHT]) {
			//player.TurnRight(delta);
		}
	}
};


Scene::Scene(const RPG::WindowSize& size, std::string guid) : internal(RPG::MakeInternalPointer<Internal>(size, guid)) {}

RPG::AssetManifest Scene::GetAssetManifest() {
	return internal->GetAssetManifest();
}

void Scene::Prepare() {
	internal->Prepare();
}

void Scene::Awake() {
	internal->Awake();
}

void Scene::Start() {
	internal->Start();
}

void Scene::Update(const float& delta) {
	internal->Update(delta);
}

void Scene::Render(RPG::IRenderer& renderer) {
	internal->Render(renderer);
}

void Scene::RenderToFrameBuffer(RPG::IRenderer &renderer, std::shared_ptr<RPG::FrameBuffer> frameBuffer, glm::vec3 clearColor, bool isGameCamera = true) {
	internal->RenderToFrameBuffer(renderer, frameBuffer, clearColor, isGameCamera);
}

void Scene::RenderLinesToFrameBuffer(RPG::IRenderer& renderer, std::shared_ptr<RPG::FrameBuffer> frameBuffer) {
	internal->RenderLinesToFrameBuffer(renderer, frameBuffer);
}

void Scene::OnWindowResized(const RPG::WindowSize& size) {
	internal->OnWindowResized(size);
}

std::shared_ptr<RPG::Hierarchy> Scene::GetHierarchy() {
	return internal->hierarchy;
}

std::string Scene::GetGuid() {
	return internal->guid;
}

bool Scene::HasLoaded() {
	return internal->hasLoaded;
}

std::shared_ptr<RPG::CameraComponent> Scene::GetCamera() {
	return internal->GetSceneCameraComponent();
}

glm::vec3 Scene::GetCameraPosition() {
	return internal->sceneCamera->GetTransform()->GetPosition();
}