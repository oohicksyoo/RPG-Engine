//
// Created by Alex on 12/14/2020.
//

#include "VulkanApplication.hpp"
#include "../../core/GraphicsWrapper.hpp"
#include "../../core/SDLWrapper.hpp"
#include "../../core/Serializer.hpp"
#include "VulkanContext.hpp"

using RPG::VulkanApplication;

namespace {
	std::unique_ptr<RPG::IScene> CreateMainScene(RPG::VulkanContext& context) {
		std::unique_ptr<RPG::IScene> scene{RPG::Serializer::LoadScene(context.GetCurrentWindowSize())};
		context.LoadAssetManifest(scene->GetAssetManifest());
		scene->Prepare();

		return scene;
	}
}

struct VulkanApplication::Internal {
	RPG::VulkanContext context;
	std::unique_ptr<RPG::IScene> scene;

	Internal() : context(RPG::VulkanContext()) {}

	RPG::IScene& GetScene() {
		if (!scene) {
			scene = ::CreateMainScene(context);
		}

		return *scene;
	}

	void Update(const float& delta) {
		GetScene().Update(delta);
	}

	void Render() {
		if (context.RenderBegin()) {
			GetScene().Render(context);
			context.RenderEnd();
		}
	}

	void OnWindowResized() {
		GetScene().OnWindowResized(context.GetCurrentWindowSize());
	}
};

VulkanApplication::VulkanApplication() : internal(RPG::MakeInternalPointer<Internal>()) {}

void VulkanApplication::Update(const float& delta) {
	internal->Update(delta);
}

void VulkanApplication::Render() {
	internal->Render();
}

void VulkanApplication::OnWindowResized() {
	internal->OnWindowResized();
}

void VulkanApplication::OnGeneralEventData(SDL_Event event) {
	//TODO: Set this up for systems like ImGUI that may need events
}
