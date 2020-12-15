//
// Created by Alex on 12/14/2020.
//

#pragma once

#include "../../core/InternalPointer.hpp"
#include "VulkanDevice.hpp"
#include <vector>

namespace RPG {
	struct VulkanCommandPool {
		public:
			VulkanCommandPool(const RPG::VulkanDevice& device);
			vk::UniqueCommandBuffer BeginCommandBuffer(const RPG::VulkanDevice& device) const;
			void EndCommandBuffer(const vk::CommandBuffer& commandBuffer, const RPG::VulkanDevice& device) const;
			std::vector<vk::UniqueCommandBuffer> CreateCommandBuffers(const RPG::VulkanDevice& device, const uint32_t& count) const;

		private:
			struct Internal;
			RPG::InternalPointer<Internal> internal;
	};
}


