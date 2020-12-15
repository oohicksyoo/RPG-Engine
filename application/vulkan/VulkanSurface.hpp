//
// Created by Alex on 12/14/2020.
//

#pragma once

#include "../../core/GraphicsWrapper.hpp"
#include "../../core/InternalPointer.hpp"
#include "../../core/SDLWindow.hpp"
#include "VulkanPhysicalDevice.hpp"

namespace RPG {
	struct VulkanSurface {
		public:
			VulkanSurface(const vk::Instance& instance, const RPG::VulkanPhysicalDevice& physicalDevice, const RPG::SDLWindow& window);
			const vk::SurfaceKHR& GetSurface() const;

		private:
			struct Internal;
			RPG::InternalPointer<Internal> internal;
	};
}


