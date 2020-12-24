//
// Created by Alex on 12/13/2020.
//

#pragma once

#include "AssetManifest.hpp"
#include "IRenderer.hpp"
#include "WindowSize.hpp"

namespace RPG {
	struct IScene {
		IScene() = default;
		virtual ~IScene() = default;
		virtual RPG::AssetManifest GetAssetManifest() = 0;
		virtual void Prepare() = 0;
		virtual void Update(const float& delta) = 0;
		virtual void Render(RPG::IRenderer& renderer) = 0;
		virtual void OnWindowResized(const RPG::WindowSize& size) = 0;
	};
}
