//
// Created by Alex on 1/5/2021.
//

#include "Scene.hpp"
#include "SDLWrapper.hpp"
#include "Log.hpp"
#include "Guid.hpp"
#include "components/CameraComponent.hpp"
#include <nlohmann/json.hpp>

#ifdef RPG_EDITOR
	#include "../../editor/EditorStats.hpp"
	#include "input/InputManager.hpp"
#endif

#include "Texture.hpp"
#include "Mesh.hpp"
#include "Content.hpp"

using RPG::Scene;
using RPG::Assets::Pipeline;
using json = nlohmann::json;

namespace {
	std::shared_ptr<RPG::GameObject> CreateSceneCamera(const RPG::WindowSize& size) {
		std::shared_ptr<RPG::GameObject> go = std::make_unique<RPG::GameObject>();
		std::shared_ptr<RPG::CameraComponent> cc = std::make_unique<RPG::CameraComponent>(static_cast<float>(size.width), static_cast<float>(size.height), go->GetTransform());
		cc->SetPitch(55);

		go->AddComponent(cc);
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
	glm::vec2 oldMousePosition;

	Internal(const RPG::WindowSize& size, std::string guid) : guid(guid),
											hasLoaded(false),
											keyboardState(SDL_GetKeyboardState(nullptr)),
											hierarchy(std::make_unique<RPG::Hierarchy>(RPG::Hierarchy())),
											sceneCamera(::CreateSceneCamera(size)),
										    gameCamera(nullptr),
											oldMousePosition({0,0}) {}

	RPG::AssetManifest GetAssetManifest() {
		json j = json::parse(RPG::Assets::LoadTextFile("assets/project/resources.projectasset"));
		for (auto [key, value] : j["Models"].items()) {
			RPG::Content::GetInstance().Load<RPG::Mesh>(value.get<std::string>());
		}

		for (auto [key, value] : j["Textures"].items()) {
			RPG::Content::GetInstance().Load<RPG::Texture>(value.get<std::string>());
		}

		hasLoaded = true;

		return RPG::AssetManifest {
				{{Pipeline::Default, Pipeline::SceneLines}}
		};
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

	void UpdateEditorScene(const float& delta) {
		#ifdef RPG_EDITOR
			auto cameraComponent = GetSceneCameraComponent();
			auto mousePosition = InputManager::GetInstance().GetMousePosition();

			if (RPG::EditorStats::GetInstance().IsMouseInSceneScreen(mousePosition)) {
				if (InputManager::GetInstance().IsKeyPressed(RPG::Input::Key::Q)) {
					RPG::EditorStats::GetInstance().SetGizmoTool(-1);
				}

				if (InputManager::GetInstance().IsKeyPressed(RPG::Input::Key::W)) {
					RPG::EditorStats::GetInstance().SetGizmoTool(0);//ImGuizmo::OPERATION::TRANSLATE;
				}

				if (InputManager::GetInstance().IsKeyPressed(RPG::Input::Key::E)) {
					RPG::EditorStats::GetInstance().SetGizmoTool(1);//ImGuizmo::OPERATION::ROTATE;
				}

				if (InputManager::GetInstance().IsKeyPressed(RPG::Input::Key::R)) {
					RPG::EditorStats::GetInstance().SetGizmoTool(2);//ImGuizmo::OPERATION::SCALE;
				}

				if (InputManager::GetInstance().IsKeyPressed(RPG::Input::Key::F)) {
					auto go = RPG::EditorStats::GetInstance().GetSelectedGameObject();
					if (go != nullptr) {
						auto t = sceneCamera->GetTransform();
						t->SetPosition(go->GetTransform()->GetPosition());
					}
				}

				if (InputManager::GetInstance().IsKeyDown(RPG::Input::Key::LeftControl)) {
					glm::vec2 mousePosition = InputManager::GetInstance().GetMousePosition();
					glm::vec2 mouseDelta = (mousePosition - oldMousePosition) * 0.003f;
					oldMousePosition = mousePosition;

					if (InputManager::GetInstance().IsMouseButtonDown(RPG::Input::MouseButton::Middle)) {
						cameraComponent->Pan(mouseDelta);
					} else if (InputManager::GetInstance().IsMouseButtonDown(RPG::Input::MouseButton::Left)) {
						cameraComponent->Rotate(mouseDelta);
					} else if (InputManager::GetInstance().IsMouseButtonDown(RPG::Input::MouseButton::Right)) {
						cameraComponent->Zoom(mouseDelta.y);
					}

					auto mouseWheel = InputManager::GetInstance().GetMouseWheel();
					if (mouseWheel != glm::vec2{0, 0}) {
						if (mouseWheel.y > 1) {
							mouseWheel.y = 1;
						} else if (mouseWheel.y < -1) {
							mouseWheel.y = -1;
						}
						cameraComponent->Zoom(mouseWheel.y * 0.5f);
					}
				}
			}

			cameraComponent->Update(delta);
			if (gameCamera != nullptr) {
				gameCamera->Update(delta);
			}
		#endif
	}

	void Render(RPG::IRenderer& renderer) {
		renderer.Render(Pipeline::Default, hierarchy, GetGameCameraMatrix());
	}

	void RenderToFrameBuffer(RPG::IRenderer& renderer, std::shared_ptr<RPG::FrameBuffer>frameBuffer, glm::vec3 clearColor, bool isGameCamera = true) {
		renderer.RenderToFrameBuffer(Pipeline::Default, hierarchy, frameBuffer, (isGameCamera) ? GetGameCameraMatrix() : GetSceneCameraMatrix(), clearColor, isGameCamera);
	}

	void RenderLinesToFrameBuffer(RPG::IRenderer& renderer, std::shared_ptr<RPG::FrameBuffer> frameBuffer) {
		renderer.RenderLinesToFrameBuffer(Pipeline::SceneLines, frameBuffer, GetSceneCameraMatrix());
	}

	void OnWindowResized(const RPG::WindowSize& size) {
		sceneCamera = ::CreateSceneCamera(size);
	}

	std::shared_ptr<RPG::CameraComponent> GetSceneCameraComponent() {
		return sceneCamera->GetComponent<std::shared_ptr<RPG::CameraComponent>, RPG::CameraComponent>(std::make_unique<RPG::CameraComponent>(1, 1, sceneCamera->GetTransform()));
	}

	std::shared_ptr<RPG::CameraComponent> GetGameCameraComponent() {
		return gameCamera->GetComponent<std::shared_ptr<RPG::CameraComponent>, RPG::CameraComponent>(std::make_unique<RPG::CameraComponent>(1, 1, gameCamera->GetTransform()));
	}

	glm::mat4 GetSceneCameraMatrix() {
		auto cameraComponent = GetSceneCameraComponent();
		return {cameraComponent->GetProjectionMatrix() * cameraComponent->GetViewMatrix(sceneCamera->GetTransform()->GetPosition())};
	}

	glm::mat4 GetGameCameraMatrix() {
		if (gameCamera != nullptr && GetGameCameraComponent() == nullptr) {
			gameCamera = nullptr;
		}

		if (gameCamera == nullptr) {
			//Attempt to find a camera component in the scene to use
			auto gc = GetGameCameraInScene();
			if (gc == nullptr) {
				return GetSceneCameraMatrix();
			}
			gameCamera = gc;
		}

		auto cameraComponent = GetGameCameraComponent();
		return {cameraComponent->GetProjectionMatrix() * cameraComponent->GetViewMatrix(gameCamera->GetTransform()->GetPosition())};
	}

	std::shared_ptr<RPG::GameObject> GetGameCameraInScene() {
		for (auto child : hierarchy->GetHierarchy()) {
			auto go = HasCameraComponent(child);
			if (go != nullptr) {
				return go;
			}
		}

		return nullptr;
	}

	std::shared_ptr<RPG::GameObject> HasCameraComponent(std::shared_ptr<RPG::GameObject> gameObject) {
		for (auto child : gameObject->GetChildren()) {
			if (HasCameraComponent(child) != nullptr) {
				return child;
			}
		}

		auto cameraComponent = gameObject->GetComponent<std::shared_ptr<RPG::CameraComponent>, RPG::CameraComponent>(std::make_unique<RPG::CameraComponent>(1280, 720, gameObject->GetTransform()));
		if (cameraComponent != nullptr) {
			return gameObject;
		}

		return nullptr;
	}

	void ProcessInput(const float& delta) {

	}
};


Scene::Scene(const RPG::WindowSize& size, std::string guid) : internal(RPG::MakeInternalPointer<Internal>(size, guid)) {}

RPG::AssetManifest Scene::GetAssetManifest() {
	return internal->GetAssetManifest();
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

void Scene::UpdateEditorScene(const float &delta) {
	internal->UpdateEditorScene(delta);
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