//
// Created by Alex on 12/14/2020.
//

#include "VulkanImage.hpp"

using RPG::VulkanImage;

namespace {
	vk::UniqueImage CreateImage(const vk::Device& device,
								const uint32_t& width,
								const uint32_t& height,
								const uint32_t& mipLevels,
								const vk::SampleCountFlagBits& sampleCount,
								const vk::Format& format,
								const vk::ImageTiling& tiling,
								const vk::ImageUsageFlags& usageFlags) {
		vk::Extent3D extent{
				width,  // Width
				height, // Height
				1       // Depth
		};

		vk::ImageCreateInfo imageInfo{
				vk::ImageCreateFlags(),       // Flags
				vk::ImageType::e2D,           // Image type
				format,                       // Format
				extent,                       // Extent
				mipLevels,                    // Mip levels
				1,                            // Array layers
				sampleCount,                  // Sample count
				tiling,                       // Tiling
				usageFlags,                   // Usage flags
				vk::SharingMode::eExclusive,  // Sharing mode
				0,                            // Queue family index count
				nullptr,                      // Queue family indices,
				vk::ImageLayout::eUndefined }; // Initial layout

		return device.createImageUnique(imageInfo);
	}

	vk::UniqueDeviceMemory AllocateImageMemory(const RPG::VulkanPhysicalDevice& physicalDevice,
											   const vk::Device& device,
											   const vk::Image& image,
											   const vk::MemoryPropertyFlags& memoryFlags) {
		// Discover what the memory requirements are for the specified image configuration.
		vk::MemoryRequirements memoryRequirements{device.getImageMemoryRequirements(image)};

		// Query the physical device to determine where to find the memory type the image requires.
		uint32_t memoryTypeIndex{physicalDevice.GetMemoryTypeIndex(memoryRequirements.memoryTypeBits, memoryFlags)};

		// Form a configuration to model what kind of memory to allocate.
		vk::MemoryAllocateInfo info{
				memoryRequirements.size, // Allocation size
				memoryTypeIndex  // Memory type index
		};

		// Request that the logical device allocate memory for our configuration.
		vk::UniqueDeviceMemory deviceMemory{device.allocateMemoryUnique(info)};

		// Bind the image to the allocated memory to associate them with each other.
		device.bindImageMemory(image, deviceMemory.get(), 0);

		// Give back the allocated memory.
		return deviceMemory;
	}

	void ApplyTransitionLayoutCommand(const RPG::VulkanDevice& device,
									  const RPG::VulkanCommandPool& commandPool,
									  const vk::PipelineStageFlags& sourceStageFlags,
									  const vk::PipelineStageFlags& destinationStageFlags,
									  const vk::ImageMemoryBarrier& barrier) {
		// Obtain a new command buffer than has been started.
		vk::UniqueCommandBuffer commandBuffer{commandPool.BeginCommandBuffer(device)};

		// Issue a 'pipeline barrier' command, using the image memory barrier as configuration
		// and the source / destination stage flags to determine where in the graphics pipeline
		// to apply the command.
		commandBuffer->pipelineBarrier(
				sourceStageFlags,
				destinationStageFlags,
				vk::DependencyFlags(),
				0, nullptr,
				0, nullptr,
				1, &barrier
				);

		// End the command buffer, causing it to be run.
		commandPool.EndCommandBuffer(commandBuffer.get(), device);
	}

	void TransitionLayout(const RPG::VulkanDevice& device,
						  const RPG::VulkanCommandPool& commandPool,
						  const vk::Image& image,
						  const vk::Format& format,
						  const uint32_t& mipLevels,
						  const vk::ImageLayout& oldLayout,
						  const vk::ImageLayout& newLayout) {
		// Create a barrier with sensible defaults - some properties will change
		// depending on the old -> new layout combinations.
		vk::ImageMemoryBarrier barrier;
		barrier.image = image;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;
		barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = mipLevels;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;

		// Scenario: undefined -> color attachment optimal
		if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eColorAttachmentOptimal) {
			barrier.dstAccessMask = vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite;

			return ::ApplyTransitionLayoutCommand(device,
												  commandPool,
												  vk::PipelineStageFlagBits::eTopOfPipe,
												  vk::PipelineStageFlagBits::eColorAttachmentOutput,
												  barrier);
		}

		// Scenario: undefined -> depth stencil attachment optimal
		if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal) {
			barrier.dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite;
			barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth;

			return ::ApplyTransitionLayoutCommand(device,
												  commandPool,
												  vk::PipelineStageFlagBits::eTopOfPipe,
												  vk::PipelineStageFlagBits::eEarlyFragmentTests,
												  barrier);
		}

		// Scenario: undefined -> transfer destination optimal
		if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal) {
			barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

			return ::ApplyTransitionLayoutCommand(device,
												  commandPool,
												  vk::PipelineStageFlagBits::eTopOfPipe,
												  vk::PipelineStageFlagBits::eTransfer,
												  barrier);
		}

		// An unknown combination might mean we need to add a new scenario to handle it.
		throw std::runtime_error("RPG::VulkanImage::TransitionLayout: Unsupported 'old' and 'new' image layout combination.");
	}
}

struct VulkanImage::Internal {
	const uint32_t width;
	const uint32_t height;
	const uint32_t mipLevels;
	const vk::Format format;
	const vk::UniqueImage image;
	const vk::UniqueDeviceMemory imageMemory;

	Internal(const RPG::VulkanCommandPool& commandPool,
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
			 const vk::ImageLayout& newLayout)
			: width(width),
			  height(height),
			  mipLevels(mipLevels),
			  format(format),
			  image(::CreateImage(device.GetDevice(), width, height, mipLevels, sampleCount, format, tiling, usageFlags)),
			  imageMemory(::AllocateImageMemory(physicalDevice, device.GetDevice(), image.get(), memoryFlags)) {
		::TransitionLayout(device, commandPool, image.get(), format, mipLevels, oldLayout, newLayout);
	}
};

VulkanImage::VulkanImage(const RPG::VulkanCommandPool& commandPool,
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
						 const vk::ImageLayout& newLayout)
		: internal(RPG::MakeInternalPointer<Internal>(commandPool,
													physicalDevice,
													device,
													width,
													height,
													mipLevels,
													sampleCount,
													format,
													tiling,
													usageFlags,
													memoryFlags,
													oldLayout,
													newLayout)) {
}

uint32_t VulkanImage::GetWidth() const {
	return internal->width;
}

uint32_t VulkanImage::GetHeight() const {
	return internal->height;
}

uint32_t VulkanImage::GetMipLevels() const {
	return internal->mipLevels;
}

const vk::Format& VulkanImage::GetFormat() const {
	return internal->format;
}

const vk::Image& VulkanImage::GetImage() const {
	return internal->image.get();
}