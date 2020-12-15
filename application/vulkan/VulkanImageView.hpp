//
// Created by Alex on 12/14/2020.
//

#pragma once

#include "../../core/GraphicsWrapper.hpp"
#include "../../core/InternalPointer.hpp"

namespace RPG {
	struct VulkanImageView {
		public:
			VulkanImageView(const vk::Device& device,
							const vk::Image& image,
							const vk::Format& format,
							const vk::ImageAspectFlags& aspectFlags,
							const uint32_t& mipLevels);
			const vk::ImageView& GetImageView() const;

		private:
			struct Internal;
			RPG::InternalPointer<Internal> internal;
	};
}


