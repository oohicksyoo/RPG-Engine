//
// Created by Alex on 12/14/2020.
//

#pragma once

#include "../../core/AssetInventory.hpp"
#include "../../core/Bitmap.hpp"
#include "../../core/GraphicsWrapper.hpp"
#include "../../core/InternalPointer.hpp"
#include "VulkanCommandPool.hpp"
#include "VulkanDevice.hpp"
#include "VulkanImageView.hpp"
#include "VulkanPhysicalDevice.hpp"

namespace RPG {
	struct VulkanTexture {
		public:
			VulkanTexture(const RPG::Assets::Texture& textureId,
						  const RPG::VulkanPhysicalDevice& physicalDevice,
						  const RPG::VulkanDevice& device,
						  const RPG::VulkanCommandPool& commandPool,
						  const RPG::Bitmap& bitmap);
			const RPG::Assets::Texture& GetTextureId() const;
			const RPG::VulkanImageView& GetImageView() const;
			const vk::Sampler& GetSampler() const;

		private:
			struct Internal;
			RPG::InternalPointer<Internal> internal;
	};
}


