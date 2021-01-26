//
// Created by Alex on 12/14/2020.
//

#pragma once

#include "../../core/AssetManifest.hpp"
#include "../../core/InternalPointer.hpp"
#include "VulkanCommandPool.hpp"
#include "VulkanDevice.hpp"
#include "VulkanMesh.hpp"
#include "VulkanPhysicalDevice.hpp"
#include "VulkanRenderContext.hpp"
#include "VulkanTexture.hpp"
#include "VulkanPipeline.hpp"
#include <string>

namespace RPG {
	struct VulkanAssetManager {
		public:
			VulkanAssetManager(const RPG::VulkanPhysicalDevice& physicalDevice,
							   const RPG::VulkanDevice& device,
							   const RPG::VulkanRenderContext& renderContext,
							   const RPG::VulkanCommandPool& commandPool);
			void LoadAssetManifest(const RPG::VulkanPhysicalDevice& physicalDevice,
								   const RPG::VulkanDevice& device,
								   const RPG::VulkanRenderContext& renderContext,
								   const RPG::AssetManifest& assetManifest);

			void ReloadContextualAssets(const RPG::VulkanPhysicalDevice& physicalDevice,
										const RPG::VulkanDevice& device,
										const RPG::VulkanRenderContext& renderContext);
			void LoadAssetManifest(const RPG::VulkanPhysicalDevice& physicalDevice,
								   const RPG::VulkanDevice& device,
								   const RPG::VulkanRenderContext& renderContext,
								   const RPG::VulkanCommandPool& commandPool,
								   const RPG::AssetManifest& assetManifest);
			const RPG::VulkanMesh& GetStaticMesh(std::string staticMesh) const;
			const RPG::VulkanTexture& GetTexture(std::string texture) const;
			const RPG::VulkanPipeline& GetPipeline(const RPG::Assets::Pipeline& pipeline) const;

		private:
			struct Internal;
			RPG::InternalPointer<Internal> internal;
	};
}


