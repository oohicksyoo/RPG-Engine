//
// Created by Alex on 12/14/2020.
//

#pragma once

#include "../../core/InternalPointer.hpp"
#include "VulkanDevice.hpp"
#include "VulkanPhysicalDevice.hpp"
#include "VulkanSwapchain.hpp"

namespace RPG {
	struct VulkanRenderPass {
		public:
			VulkanRenderPass(const RPG::VulkanPhysicalDevice& physicalDevice,
							 const RPG::VulkanDevice& device,
							 const RPG::VulkanSwapchain& swapchain);
			const vk::RenderPass& GetRenderPass() const;

		private:
			struct Internal;
			RPG::InternalPointer<Internal> internal;
	};
}


