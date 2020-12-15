//
// Created by Alex on 12/14/2020.
//

#pragma once

#include "../../core/GraphicsWrapper.hpp"
#include "../../core/InternalPointer.hpp"
#include "VulkanCommandPool.hpp"
#include "VulkanDevice.hpp"
#include "VulkanPhysicalDevice.hpp"

namespace RPG {
	struct VulkanBuffer {
		public:
			VulkanBuffer(const RPG::VulkanPhysicalDevice& physicalDevice,
						 const RPG::VulkanDevice& device,
						 const vk::DeviceSize& size,
						 const vk::BufferUsageFlags& bufferFlags,
						 const vk::MemoryPropertyFlags& memoryFlags,
						 const void* dataSource);
			const vk::Buffer& GetBuffer() const;
			static RPG::VulkanBuffer CreateDeviceLocalBuffer(const RPG::VulkanPhysicalDevice& physicalDevice,
															 const RPG::VulkanDevice& device,
															 const RPG::VulkanCommandPool& commandPool,
															 const vk::DeviceSize& size,
															 const vk::BufferUsageFlags& bufferFlags,
															 const void* dataSource);

		private:
			struct Internal;
			RPG::InternalPointer<Internal> internal;
	};
}


