//
// Created by Alex on 12/14/2020.
//

#pragma once

#include "../../core/GraphicsWrapper.hpp"
#include "../../core/InternalPointer.hpp"
#include "../../core/StaticMeshInstance.hpp"
#include "VulkanDevice.hpp"
#include "VulkanPhysicalDevice.hpp"
#include <string>
#include <vector>

namespace RPG {
	struct VulkanAssetManager;

	struct VulkanPipeline {
		public:
			VulkanPipeline(const RPG::VulkanPhysicalDevice& physicalDevice,
						   const RPG::VulkanDevice& device,
						   const std::string& shaderName,
						   const vk::Viewport& viewport,
						   const vk::Rect2D& scissor,
						   const vk::RenderPass& renderPass);
			void Render(const RPG::VulkanDevice& device,
						const vk::CommandBuffer& commandBuffer,
						const RPG::VulkanAssetManager& assetManager,
						const std::vector<RPG::StaticMeshInstance>& staticMeshInstances) const;

		private:
			struct Internal;
			RPG::InternalPointer<Internal> internal;
	};
}


