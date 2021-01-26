//
// Created by Alex on 12/14/2020.
//

#pragma once

#include "../../core/GraphicsWrapper.hpp"
#include "../../core/InternalPointer.hpp"
#include "../../core/Mesh.hpp"
#include "VulkanCommandPool.hpp"
#include "VulkanDevice.hpp"
#include "VulkanPhysicalDevice.hpp"
#include <memory>

namespace RPG {
	struct VulkanMesh {
		public:
			VulkanMesh(const RPG::VulkanPhysicalDevice& physicalDevice,
					   const RPG::VulkanDevice& device,
					   const RPG::VulkanCommandPool& commandPool,
					   std::shared_ptr<RPG::Mesh> mesh);
			const vk::Buffer& GetVertexBuffer() const;
			const vk::Buffer& GetIndexBuffer() const;
			const uint32_t& GetNumIndices() const;

		private:
			struct Internal;
			RPG::InternalPointer<Internal> internal;
	};
}


