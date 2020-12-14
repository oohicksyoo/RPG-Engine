//
// Created by Alex on 12/13/2020.
//


#include "AssetManifest.hpp"
#include "Renderer.hpp"
#include "WindowSize.hpp"

namespace RPG {
	struct Scene {
		Scene() = default;
		virtual ~Scene() = default;
		virtual RPG::AssetManifest GetAssetManifest() = 0;
		virtual void Prepare() = 0;
		virtual void Update(const float& delta) = 0;
		virtual void Render(RPG::Renderer& renderer) = 0;
		virtual void OnWindowResized(const RPG::WindowSize& size) = 0;
	};
}
