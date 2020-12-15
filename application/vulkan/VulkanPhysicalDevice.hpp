//
// Created by Alex on 12/14/2020.
//

#pragma once

#include "../../core/GraphicsWrapper.hpp"
#include "../../core/InternalPointer.hpp"

namespace RPG {
	struct VulkanPhysicalDevice {
		public:
			VulkanPhysicalDevice(const vk::Instance& instance);
			const vk::PhysicalDevice& GetPhysicalDevice() const;
			const vk::SampleCountFlagBits GetMultiSamplingLevel() const;
			const vk::Format GetDepthFormat() const;
			uint32_t GetMemoryTypeIndex(const uint32_t& filter, const vk::MemoryPropertyFlags& flags) const;
			bool IsShaderMultiSamplingSupported() const;
			bool IsAnisotropicFilteringSupported() const;

		private:
			struct Internal;
			RPG::InternalPointer<Internal> internal;
	};
}


