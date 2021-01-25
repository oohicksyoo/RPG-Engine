//
// Created by Alex on 12/13/2020.
//

#pragma once

#include "AssetManifest.hpp"
#include "IRenderer.hpp"
#include "FrameBuffer.hpp"
#include "WindowSize.hpp"
#include "Hierarchy.hpp"
#include "components/CameraComponent.hpp"
#include <vector>
#include <string>

namespace RPG {
	struct IScene {
		IScene() = default;
		virtual ~IScene() = default;
		virtual RPG::AssetManifest GetAssetManifest() = 0;
		virtual void Awake() = 0;
		virtual void Start() = 0;
		virtual void Update(const float& delta) = 0;
		virtual void UpdateEditorScene(const float& delta) = 0;
		virtual void Render(RPG::IRenderer& renderer) = 0;
		virtual void RenderToFrameBuffer(RPG::IRenderer& renderer, std::shared_ptr<RPG::FrameBuffer> frameBuffer, glm::vec3 clearColor, bool isGameCamera = true) = 0;
		virtual void RenderLinesToFrameBuffer(RPG::IRenderer& renderer, std::shared_ptr<RPG::FrameBuffer> frameBuffer) = 0;
		virtual void OnWindowResized(const RPG::WindowSize& size) = 0;
		virtual std::shared_ptr<RPG::Hierarchy> GetHierarchy() = 0;
		virtual std::string GetGuid() = 0;
		virtual bool HasLoaded() = 0;
		virtual std::shared_ptr<RPG::CameraComponent> GetCamera() = 0;
		virtual glm::vec3 GetCameraPosition() = 0;
	};
}
