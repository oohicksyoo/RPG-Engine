//
// Created by Alex on 12/14/2020.
//

#include "VulkanCommandPool.hpp"

using RPG::VulkanCommandPool;

namespace {
	vk::UniqueCommandPool CreateCommandPool(const RPG::VulkanDevice& device) {
		vk::CommandPoolCreateInfo info{vk::CommandPoolCreateFlagBits::eResetCommandBuffer, device.GetGraphicsQueueIndex()};
		return device.GetDevice().createCommandPoolUnique(info);
	}

	vk::UniqueCommandBuffer BeginCommandBuffer(const vk::CommandPool& commandPool, const RPG::VulkanDevice& device) {
		// Describe how to create our command buffer - we only create 1.
		vk::CommandBufferAllocateInfo allocateInfo{
				commandPool,                      // Command pool
				vk::CommandBufferLevel::ePrimary, // Level
				1 // Command buffer count
		};

		// Create and move the first (and only) command buffer that gets created by the device.
		vk::UniqueCommandBuffer commandBuffer{std::move(device.GetDevice().allocateCommandBuffersUnique(allocateInfo)[0])};

		// Define how this command buffer should begin.
		vk::CommandBufferBeginInfo beginInfo{
				vk::CommandBufferUsageFlagBits::eOneTimeSubmit, // Flags
				nullptr                                         // Inheritance info
		};

		// Request the command buffer to begin itself.
		commandBuffer->begin(beginInfo);

		// The caller will now take ownership of the command buffer and is
		// responsible for invoking the 'endCommandBuffer' upon it.
		return commandBuffer;
	}

	void EndCommandBuffer(const vk::CommandBuffer& commandBuffer, const RPG::VulkanDevice& device) {
		// Ask the command buffer to end itself.
		commandBuffer.end();

		// Configure a submission object to send to the graphics queue to wait
		// for the command buffer to have been completed.
		vk::SubmitInfo submitInfo{
				0,              // Wait semaphore count
				nullptr,        // Wait semaphores
				nullptr,        // Wait destination stage mask
				1,              // Command buffer count
				&commandBuffer, // Command buffers,
				0,              // Signal semaphore count
				nullptr         // Signal semaphores
		};

		// Ask the graphics queue to take the submission object which will declare
		// the command buffer to wait on, then wait until the graphics queue is
		// idle, indicating that the command buffer is complete.
		device.GetGraphicsQueue().submit(1, &submitInfo, vk::Fence());
		device.GetGraphicsQueue().waitIdle();
	}

	std::vector<vk::UniqueCommandBuffer> CreateCommandBuffers(const vk::Device& device,
															  const vk::CommandPool& commandPool,
															  const uint32_t& count) {
		vk::CommandBufferAllocateInfo info{
				commandPool,                      // Command pool
				vk::CommandBufferLevel::ePrimary, // Level
				count                             // Command buffer count
		};

		return device.allocateCommandBuffersUnique(info);
	}
}

struct VulkanCommandPool::Internal {
	const vk::UniqueCommandPool commandPool;

	Internal(const RPG::VulkanDevice& device) : commandPool(::CreateCommandPool(device)) {}
};

VulkanCommandPool::VulkanCommandPool(const RPG::VulkanDevice& device) : internal(RPG::MakeInternalPointer<Internal>(device)) {}

vk::UniqueCommandBuffer VulkanCommandPool::BeginCommandBuffer(const RPG::VulkanDevice& device) const {
	return ::BeginCommandBuffer(internal->commandPool.get(), device);
}

void VulkanCommandPool::EndCommandBuffer(const vk::CommandBuffer& commandBuffer, const RPG::VulkanDevice& device) const {
	::EndCommandBuffer(commandBuffer, device);
}

std::vector<vk::UniqueCommandBuffer> VulkanCommandPool::CreateCommandBuffers(const RPG::VulkanDevice& device,
																			 const uint32_t& count) const {
	return ::CreateCommandBuffers(device.GetDevice(), internal->commandPool.get(), count);
}