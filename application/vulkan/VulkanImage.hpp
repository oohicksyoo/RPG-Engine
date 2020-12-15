//
// Created by Alex on 12/14/2020.
//

#pragma once

#include "../../core/GraphicsWrapper.hpp"
#include "../../core/InternalPointer.hpp"
#include "VulkanDevice.hpp"
#include "VulkanPhysicalDevice.hpp"
#include "VulkanCommandPool.hpp"

namespace RPG {
	struct VulkanImage {
		public:
			VulkanImage(const RPG::VulkanCommandPool& commandPool,
						const RPG::VulkanPhysicalDevice& physicalDevice,
						const RPG::VulkanDevice& device,
						const uint32_t& width,
						const uint32_t& height,
						const uint32_t& mipLevels,
						const vk::SampleCountFlagBits& sampleCount,
						const vk::Format& format,
						const vk::ImageTiling& tiling,
						const vk::ImageUsageFlags& usageFlags,
						const vk::MemoryPropertyFlags& memoryFlags,
						const vk::ImageLayout& oldLayout,
						const vk::ImageLayout& newLayout);
			uint32_t GetWidth() const;
			uint32_t GetHeight() const;
			uint32_t GetMipLevels() const;
			const vk::Format& GetFormat() const;
			const vk::Image& GetImage() const;

		private:
			struct Internal;
			RPG::InternalPointer<Internal> internal;
	};
}


