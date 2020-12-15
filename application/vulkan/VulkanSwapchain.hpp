//
// Created by Alex on 12/14/2020.
//

#pragma once

#include "../../core/GraphicsWrapper.hpp"
#include "../../core/InternalPointer.hpp"
#include "../../core/SDLWindow.hpp"
#include "VulkanDevice.hpp"
#include "VulkanImageView.hpp"
#include "VulkanPhysicalDevice.hpp"
#include "VulkanSurface.hpp"
#include <vector>

namespace RPG {
	struct VulkanSwapchain {
		public:
			VulkanSwapchain(const RPG::SDLWindow& window,
							const RPG::VulkanPhysicalDevice& physicalDevice,
							const RPG::VulkanDevice& device,
							const RPG::VulkanSurface& surface,
							const vk::SwapchainKHR& oldSwapchain);
			const vk::SwapchainKHR& GetSwapchain() const;
			const std::vector<RPG::VulkanImageView>& GetImageViews() const;
			const vk::Format& GetColorFormat() const;
			const vk::Extent2D& GetExtent() const;
			uint32_t GetImageCount() const;

		private:
			struct Internal;
			RPG::InternalPointer<Internal> internal;
	};
}


