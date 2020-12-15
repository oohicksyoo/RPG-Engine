//
// Created by Alex on 12/14/2020.
//

#include "VulkanBuffer.hpp"

using RPG::VulkanBuffer;

namespace {
	vk::UniqueBuffer CreateBuffer(const RPG::VulkanDevice& device,
								  const vk::DeviceSize& size,
								  const vk::BufferUsageFlags& bufferFlags) {
		vk::BufferCreateInfo info{
				vk::BufferCreateFlags(),     // Flags
				size,                        // Size
				bufferFlags,                 // Buffer usage flags
				vk::SharingMode::eExclusive, // Sharing mode
				0,                           // Queue family index count
				nullptr };                    // Queue family indices

		return device.GetDevice().createBufferUnique(info);
	}

	vk::UniqueDeviceMemory AllocateMemory(const RPG::VulkanPhysicalDevice& physicalDevice,
										  const RPG::VulkanDevice& device,
										  const vk::Buffer& buffer,
										  const vk::MemoryPropertyFlags& memoryFlags) {
		vk::MemoryRequirements memoryRequirements{device.GetDevice().getBufferMemoryRequirements(buffer)};
		uint32_t memoryTypeIndex{physicalDevice.GetMemoryTypeIndex(memoryRequirements.memoryTypeBits, memoryFlags)};
		vk::MemoryAllocateInfo info{
				memoryRequirements.size, // Allocation size
				memoryTypeIndex // Memory type index
		};

		return device.GetDevice().allocateMemoryUnique(info);
	}
}

struct VulkanBuffer::Internal {
	const vk::UniqueBuffer buffer;
	const vk::UniqueDeviceMemory deviceMemory;

	Internal(const RPG::VulkanPhysicalDevice& physicalDevice,
			 const RPG::VulkanDevice& device,
			 const vk::DeviceSize& size,
			 const vk::BufferUsageFlags& bufferFlags,
			 const vk::MemoryPropertyFlags& memoryFlags,
			 const void* dataSource) : buffer(::CreateBuffer(device, size, bufferFlags)),
									   deviceMemory(::AllocateMemory(physicalDevice, device, buffer.get(), memoryFlags)) {

		// Take the buffer and the allocated memory and bind them together.
		device.GetDevice().bindBufferMemory(buffer.get(), deviceMemory.get(), 0);

		// Take the datasource and copy it into our allocated memory block.
		if (dataSource) {
			void* mappedMemory{device.GetDevice().mapMemory(deviceMemory.get(), 0, size)};
			std::memcpy(mappedMemory, dataSource, static_cast<size_t>(size));
			device.GetDevice().unmapMemory(deviceMemory.get());
		}
	}
};

VulkanBuffer::VulkanBuffer(const RPG::VulkanPhysicalDevice& physicalDevice,
						   const RPG::VulkanDevice& device,
						   const vk::DeviceSize& size,
						   const vk::BufferUsageFlags& bufferFlags,
						   const vk::MemoryPropertyFlags& memoryFlags,
						   const void* dataSource)
		: internal(RPG::MakeInternalPointer<Internal>(physicalDevice, device, size, bufferFlags, memoryFlags, dataSource)) {
}

const vk::Buffer& VulkanBuffer::GetBuffer() const {
	return internal->buffer.get();
}

RPG::VulkanBuffer VulkanBuffer::CreateDeviceLocalBuffer(const RPG::VulkanPhysicalDevice& physicalDevice,
														const RPG::VulkanDevice& device,
														const RPG::VulkanCommandPool& commandPool,
														const vk::DeviceSize& size,
														const vk::BufferUsageFlags& bufferFlags,
														const void* dataSource) {
	RPG::VulkanBuffer stagingBuffer{
			physicalDevice,
			device,
			size,
			vk::BufferUsageFlagBits::eTransferSrc,
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
			dataSource
	};

	RPG::VulkanBuffer deviceLocalBuffer{
			physicalDevice,
			device,
			size,
			vk::BufferUsageFlagBits::eTransferDst | bufferFlags,
			vk::MemoryPropertyFlagBits::eDeviceLocal,
			nullptr
	};

	vk::UniqueCommandBuffer commandBuffer{commandPool.BeginCommandBuffer(device)};

	vk::BufferCopy copyRegion{
			0,     // Source offset
			0,     // Destination offset
			size
	}; // Size

	commandBuffer->copyBuffer(stagingBuffer.GetBuffer(), deviceLocalBuffer.GetBuffer(), 1, &copyRegion);
	commandPool.EndCommandBuffer(commandBuffer.get(), device);

	return deviceLocalBuffer;
}