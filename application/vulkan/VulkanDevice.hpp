//
// Created by Alex on 12/14/2020.
//

#pragma once

#include "../../core/GraphicsWrapper.hpp"
#include "../../core/InternalPointer.hpp"
#include "VulkanPhysicalDevice.hpp"
#include "VulkanSurface.hpp"
#include <vector>

namespace RPG {
	struct VulkanDevice {
		public:
			VulkanDevice(const RPG::VulkanPhysicalDevice& physicalDevice, const RPG::VulkanSurface& surface);
			const vk::Device& GetDevice() const;
			uint32_t GetGraphicsQueueIndex() const;
			uint32_t GetPresentationQueueIndex() const;
			bool HasDiscretePresentationQueue() const;
			const vk::Queue& GetGraphicsQueue() const;
			std::vector<vk::UniqueSemaphore> CreateSemaphores(const uint32_t& count) const;
			std::vector<vk::UniqueFence> CreateFences(const uint32_t& count) const;
			const vk::Queue& GetPresentationQueue() const;
			vk::UniqueShaderModule CreateShaderModule(const std::vector<char>& shaderCode) const;

		private:
			struct Internal;
			RPG::InternalPointer<Internal> internal;
	};
}


