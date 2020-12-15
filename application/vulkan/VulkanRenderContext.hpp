//
// Created by Alex on 12/14/2020.
//

#pragma once

#include "../../core/InternalPointer.hpp"
#include "../../core/SDLWindow.hpp"
#include "VulkanDevice.hpp"
#include "VulkanPhysicalDevice.hpp"
#include "VulkanSurface.hpp"
#include "VulkanCommandPool.hpp"

namespace RPG {
	struct VulkanRenderContext {
		public:
			VulkanRenderContext(const RPG::SDLWindow& window,
								const RPG::VulkanPhysicalDevice& physicalDevice,
								const RPG::VulkanDevice& device,
								const RPG::VulkanSurface& surface,
								const RPG::VulkanCommandPool& commandPool,
								const vk::SwapchainKHR& oldSwapchain = vk::SwapchainKHR());
			bool RenderBegin(const RPG::VulkanDevice& device);
			bool RenderEnd(const RPG::VulkanDevice& device);
			RPG::VulkanRenderContext Recreate(const RPG::SDLWindow& window,
											  const RPG::VulkanPhysicalDevice& physicalDevice,
											  const RPG::VulkanDevice& device,
											  const RPG::VulkanSurface& surface,
											  const RPG::VulkanCommandPool& commandPool);
			const vk::Viewport& GetViewport() const;
			const vk::Rect2D& GetScissor() const;
			const vk::RenderPass& GetRenderPass() const;
			const vk::CommandBuffer& GetActiveCommandBuffer() const;

		private:
			struct Internal;
			RPG::InternalPointer<Internal> internal;
	};
}


