//
// Created by Alex on 1/5/2021.
//

#include "Scene.hpp"
#include "PerspectiveCamera.hpp"
#include "SDLWrapper.hpp"
#include "Log.hpp"
#include "Guid.hpp"

using RPG::Scene;
using RPG::Assets::Pipeline;
using RPG::Assets::StaticMesh;
using RPG::Assets::Texture;

namespace {
	RPG::PerspectiveCamera CreateCamera(const RPG::WindowSize& size) {
		return RPG::PerspectiveCamera(static_cast<float>(size.width), static_cast<float>(size.height));
	}
}

struct Scene::Internal {
	std::string guid;
	bool hasLoaded;
	RPG::PerspectiveCamera camera;
	const uint8_t* keyboardState;
	std::shared_ptr<RPG::Hierarchy> hierarchy;

	Internal(const RPG::WindowSize& size, std::string guid) : guid(guid),
											hasLoaded(false),
											camera(::CreateCamera(size)),
											keyboardState(SDL_GetKeyboardState(nullptr)),
											hierarchy(std::make_unique<RPG::Hierarchy>(RPG::Hierarchy())) {}

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
		camera.Configure(glm::vec3{ 0.0f, 6.0f, 10.0f }, glm::vec3{ 0.0f, 0.0f, 1.0f }); //TODO: Replace this with current Camera.Main


		for (auto gameObject : hierarchy->GetHierarchy()) {
			gameObject->Update(delta);
		}
	}

	void Render(RPG::IRenderer& renderer) {
		renderer.Render(Pipeline::Default, hierarchy, {camera.GetProjectionMatrix() * camera.GetViewMatrix()});
	}

	void RenderToFrameBuffer(RPG::IRenderer& renderer, std::shared_ptr<RPG::FrameBuffer>frameBuffer, glm::vec3 clearColor) {
		renderer.RenderToFrameBuffer(Pipeline::Default, hierarchy, frameBuffer, {camera.GetProjectionMatrix() * camera.GetViewMatrix()}, clearColor);
	}

	void RenderLinesToFrameBuffer(RPG::IRenderer& renderer, std::shared_ptr<RPG::FrameBuffer> frameBuffer) {
		renderer.RenderLinesToFrameBuffer(Pipeline::SceneLines, frameBuffer, {camera.GetProjectionMatrix() * camera.GetViewMatrix()});
	}

	void OnWindowResized(const RPG::WindowSize& size) {
		camera = ::CreateCamera(size);
	}

	void ProcessInput(const float& delta) {
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

void Scene::RenderToFrameBuffer(RPG::IRenderer &renderer, std::shared_ptr<RPG::FrameBuffer> frameBuffer, glm::vec3 clearColor) {
	internal->RenderToFrameBuffer(renderer, frameBuffer, clearColor);
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