//
// Created by Alex on 12/14/2020.
//

#include "VulkanTexture.hpp"
#include "VulkanBuffer.hpp"
#include "VulkanImage.hpp"
#include <cmath>

using RPG::VulkanTexture;

namespace {
	void GenerateMipMaps(const RPG::VulkanDevice& device,
						 const RPG::VulkanCommandPool& commandPool,
						 const RPG::VulkanImage& image) {
		vk::ImageSubresourceRange barrierSubresourceRange{
				vk::ImageAspectFlagBits::eColor, // Aspect mask
				0,                               // Base mip level
				1,                               // Level count
				0,                               // Base array layer
				1 };                              // Layer count

		vk::ImageMemoryBarrier barrier{
				vk::AccessFlags(),           // Source access mask
				vk::AccessFlags(),           // Destination access mask
				vk::ImageLayout::eUndefined, // Old layout
				vk::ImageLayout::eUndefined, // New layout
				VK_QUEUE_FAMILY_IGNORED,     // Source queue family index
				VK_QUEUE_FAMILY_IGNORED,     // Destination queue family index
				image.GetImage(),            // Image
				barrierSubresourceRange };    // Subresource range

		vk::UniqueCommandBuffer commandBuffer{ commandPool.BeginCommandBuffer(device) };

		int32_t mipWidth{ static_cast<int32_t>(image.GetWidth()) };
		int32_t mipHeight{ static_cast<int32_t>(image.GetHeight()) };
		uint32_t mipLevels{ image.GetMipLevels() };

		for (uint32_t mipLevel = 1; mipLevel < mipLevels; mipLevel++) {
			barrier.subresourceRange.baseMipLevel = mipLevel - 1;
			barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
			barrier.newLayout = vk::ImageLayout::eTransferSrcOptimal;
			barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
			barrier.dstAccessMask = vk::AccessFlagBits::eTransferRead;

			commandBuffer->pipelineBarrier(vk::PipelineStageFlagBits::eTransfer,
										   vk::PipelineStageFlagBits::eTransfer,
										   vk::DependencyFlags(),
										   0, nullptr,
										   0, nullptr,
										   1, &barrier);

			vk::ImageSubresourceLayers sourceSubresource{
					vk::ImageAspectFlagBits::eColor, // Aspect mask
					mipLevel - 1,                    // Mip level
					0,                               // Base array layer
					1 };                              // Layer count

			std::array<vk::Offset3D, 2> sourceOffsets{
					vk::Offset3D{0, 0, 0},
					vk::Offset3D{mipWidth, mipHeight, 1} };

			vk::ImageSubresourceLayers destinationSubresource{
					vk::ImageAspectFlagBits::eColor, // Aspect mask
					mipLevel,                        // Mip level
					0,                               // Base array layer
					1 };                              // Layer count

			std::array<vk::Offset3D, 2> destinationOffsets{
					vk::Offset3D{0, 0, 0},
					vk::Offset3D{mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1} };

			vk::ImageBlit blit{
					sourceSubresource,      // Source subresource
					sourceOffsets,          // Source offsets
					destinationSubresource, // Destination subresource
					destinationOffsets };    // Destination offsets

			commandBuffer->blitImage(image.GetImage(), vk::ImageLayout::eTransferSrcOptimal,
									 image.GetImage(), vk::ImageLayout::eTransferDstOptimal,
									 1, &blit,
									 vk::Filter::eLinear
									 );

			barrier.oldLayout = vk::ImageLayout::eTransferSrcOptimal;
			barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
			barrier.srcAccessMask = vk::AccessFlagBits::eTransferRead;
			barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

			commandBuffer->pipelineBarrier(vk::PipelineStageFlagBits::eTransfer,
										   vk::PipelineStageFlagBits::eFragmentShader,
										   vk::DependencyFlags(),
										   0, nullptr,
										   0, nullptr,
										   1, &barrier
										   );

			if (mipWidth > 1) {
				mipWidth /= 2;
			}

			if (mipHeight > 1) {
				mipHeight /= 2;
			}
		}

		barrier.subresourceRange.baseMipLevel = mipLevels - 1;
		barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
		barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
		barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
		barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

		commandBuffer->pipelineBarrier(
				vk::PipelineStageFlagBits::eTransfer,
				vk::PipelineStageFlagBits::eFragmentShader,
				vk::DependencyFlags(),
				0, nullptr,
				0, nullptr,
				1, &barrier);

		commandPool.EndCommandBuffer(commandBuffer.get(), device);
	}

	RPG::VulkanImage CreateImage(const RPG::VulkanPhysicalDevice& physicalDevice,
								 const RPG::VulkanDevice& device,
								 const RPG::VulkanCommandPool& commandPool,
								 std::shared_ptr<RPG::Bitmap> bitmap) {
		uint32_t imageWidth{ bitmap->GetWidth() };
		uint32_t imageHeight{ bitmap->GetHeight() };
		#undef max
		uint32_t mipLevels{ static_cast<uint32_t>(std::floor(std::log2(std::max(imageWidth, imageHeight)))) + 1 };
		vk::DeviceSize bufferSize{ imageWidth * imageHeight * 4 };

		RPG::VulkanBuffer stagingBuffer{
				physicalDevice,
				device,
				bufferSize,
				vk::BufferUsageFlagBits::eTransferSrc,
				vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
				bitmap->GetPixelData()
		};

		RPG::VulkanImage image{
				commandPool,
				physicalDevice,
				device,
				imageWidth,
				imageHeight,
				mipLevels,
				vk::SampleCountFlagBits::e1,
				vk::Format::eR8G8B8A8Unorm,
				vk::ImageTiling::eOptimal,
				vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eSampled,
				vk::MemoryPropertyFlagBits::eDeviceLocal,
				vk::ImageLayout::eUndefined,
				vk::ImageLayout::eTransferDstOptimal
		};

		vk::UniqueCommandBuffer commandBuffer{ commandPool.BeginCommandBuffer(device) };

		vk::ImageSubresourceLayers imageSubresource{
				vk::ImageAspectFlagBits::eColor, // Aspect mask
				0,                               // Mip level
				0,                               // Base array layer
				1 };                              // Layer count

		vk::Extent3D imageExtent{
				imageWidth,  // Width
				imageHeight, // Height
				1 };          // Depth

		vk::BufferImageCopy bufferImageCopy{
				0,                // Buffer offset
				0,                // Buffer row length
				0,                // Buffer image height
				imageSubresource, // Image subresource
				vk::Offset3D(),   // Image offset
				imageExtent };     // Image extent

		commandBuffer->copyBufferToImage(stagingBuffer.GetBuffer(),
										 image.GetImage(),
										 vk::ImageLayout::eTransferDstOptimal,
										 1,
										 &bufferImageCopy);

		commandPool.EndCommandBuffer(commandBuffer.get(), device);

		::GenerateMipMaps(device, commandPool, image);

		return image;
	}

	RPG::VulkanImageView CreateImageView(const RPG::VulkanDevice& device, const RPG::VulkanImage& image) {
		return RPG::VulkanImageView(device.GetDevice(),
									image.GetImage(),
									image.GetFormat(),
									vk::ImageAspectFlagBits::eColor,
									image.GetMipLevels());
	}

	vk::UniqueSampler CreateSampler(const RPG::VulkanPhysicalDevice& physicalDevice,
									const RPG::VulkanDevice& device,
									const RPG::VulkanImage& image) {
		float maxLod{ static_cast<float>(image.GetMipLevels()) };
		vk::Bool32 anisotropyEnabled = physicalDevice.IsAnisotropicFilteringSupported() ? VK_TRUE : VK_FALSE;

		vk::SamplerCreateInfo info{
				vk::SamplerCreateFlags(),         // Flags
				vk::Filter::eLinear,              // Mag filter
				vk::Filter::eLinear,              // Min filter
				vk::SamplerMipmapMode::eLinear,   // Mipmap mode
				vk::SamplerAddressMode::eRepeat,  // Address mode U
				vk::SamplerAddressMode::eRepeat,  // Address mode V
				vk::SamplerAddressMode::eRepeat,  // Address mode W
				0.0f,                             // Mip LOD bias
				anisotropyEnabled,                // Anisotropy enabled
				anisotropyEnabled ? 8.0f : 1.0f,  // Max anisotropy
				VK_FALSE,                         // Compare enable
				vk::CompareOp::eNever,            // Compare op
				0.0f,                             // Min LOD
				maxLod,                           // Max LOD
				vk::BorderColor::eIntOpaqueBlack, // Border color
				VK_FALSE };                        // UnnormalizedCoordinates

		return device.GetDevice().createSamplerUnique(info);
	}
}

struct VulkanTexture::Internal {
	std::string textureId;
	const RPG::VulkanImage image;
	const RPG::VulkanImageView imageView;
	const vk::UniqueSampler sampler;

	Internal(std::string textureId,
			 const RPG::VulkanPhysicalDevice& physicalDevice,
			 const RPG::VulkanDevice& device,
			 const RPG::VulkanCommandPool& commandPool,
			 std::shared_ptr<RPG::Bitmap> bitmap)
			: textureId(textureId),
			  image(::CreateImage(physicalDevice, device, commandPool, bitmap)),
			  imageView(::CreateImageView(device, image)),
			  sampler(::CreateSampler(physicalDevice, device, image)) {
	}
};

VulkanTexture::VulkanTexture(std::string textureId,
							 const RPG::VulkanPhysicalDevice& physicalDevice,
							 const RPG::VulkanDevice& device,
							 const RPG::VulkanCommandPool& commandPool,
							 std::shared_ptr<RPG::Bitmap> bitmap)
		: internal(RPG::MakeInternalPointer<Internal>(textureId,
													physicalDevice,
													device,
													commandPool,
													bitmap)) {
}

std::string VulkanTexture::GetTextureId() const {
	return internal->textureId;
}

const RPG::VulkanImageView& VulkanTexture::GetImageView() const {
	return internal->imageView;
}

const vk::Sampler& VulkanTexture::GetSampler() const {
	return internal->sampler.get();
}