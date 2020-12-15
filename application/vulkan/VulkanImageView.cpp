//
// Created by Alex on 12/14/2020.
//

#include "VulkanImageView.hpp"

using RPG::VulkanImageView;

namespace {
	vk::UniqueImageView CreateImageView(const vk::Device& device,
										const vk::Image& image,
										const vk::Format& format,
										const vk::ImageAspectFlags& aspectFlags,
										const uint32_t& mipLevels) {
		vk::ImageSubresourceRange subresourceRangeInfo{
				aspectFlags, // Flags
				0,           // Base mip level
				mipLevels,   // Mip level count
				0,           // Base array layer
				1            // Layer count
		};

		vk::ImageViewCreateInfo createInfo{
				vk::ImageViewCreateFlags(), // Flags
				image,                      // Image
				vk::ImageViewType::e2D,     // View type
				format,                     // Format
				vk::ComponentMapping(),     // Components
				subresourceRangeInfo        // Subresource range
		};

		return device.createImageViewUnique(createInfo);
	}
}

struct VulkanImageView::Internal {
	const vk::UniqueImageView imageView;

	Internal(const vk::Device& device,
			 const vk::Image& image,
			 const vk::Format& format,
			 const vk::ImageAspectFlags& aspectFlags,
			 const uint32_t& mipLevels)
			: imageView(::CreateImageView(device, image, format, aspectFlags, mipLevels)) {}
};

VulkanImageView::VulkanImageView(const vk::Device& device,
								 const vk::Image& image,
								 const vk::Format& format,
								 const vk::ImageAspectFlags& aspectFlags,
								 const uint32_t& mipLevels)
		: internal(RPG::MakeInternalPointer<Internal>(device, image, format, aspectFlags, mipLevels)) {
}

const vk::ImageView& VulkanImageView::GetImageView() const {
	return internal->imageView.get();
}