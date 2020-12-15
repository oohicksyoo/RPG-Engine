//
// Created by Alex on 12/14/2020.
//

#pragma once

#include "../../core/AssetManifest.hpp"
#include "../../core/InternalPointer.hpp"
#include "../../core/Renderer.hpp"
#include "../../core/WindowSize.hpp"

namespace RPG {
	struct VulkanContext : public RPG::Renderer {
		public:
			VulkanContext();
			void LoadAssetManifest(const RPG::AssetManifest& assetManifest);
			bool RenderBegin();
			void Render(const RPG::Assets::Pipeline& pipeline, const std::vector<RPG::StaticMeshInstance>& staticMeshInstances) override;
			void RenderEnd();
			RPG::WindowSize GetCurrentWindowSize() const;

		private:
			struct Internal;
			RPG::InternalPointer<Internal> internal;
	};
}


