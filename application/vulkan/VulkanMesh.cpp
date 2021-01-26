//
// Created by Alex on 12/14/2020.
//

#include "VulkanMesh.hpp"
#include "VulkanBuffer.hpp"

using RPG::VulkanMesh;

namespace {
	RPG::VulkanBuffer CreateVertexBuffer(const RPG::VulkanPhysicalDevice& physicalDevice,
										 const RPG::VulkanDevice& device,
										 const RPG::VulkanCommandPool& commandPool,
										 std::shared_ptr<RPG::Mesh> mesh) {
		return RPG::VulkanBuffer::CreateDeviceLocalBuffer(physicalDevice,
														  device,
														  commandPool,
														  sizeof(RPG::Vertex) * mesh->GetNumVertices(),
														  vk::BufferUsageFlagBits::eVertexBuffer,
														  mesh->GetVertices().data());
	}

	RPG::VulkanBuffer CreateIndexBuffer(const RPG::VulkanPhysicalDevice& physicalDevice,
										const RPG::VulkanDevice& device,
										const RPG::VulkanCommandPool& commandPool,
										std::shared_ptr<RPG::Mesh> mesh) {
		return RPG::VulkanBuffer::CreateDeviceLocalBuffer(physicalDevice,
														  device,
														  commandPool,
														  sizeof(uint32_t) * mesh->GetNumIndices(),
														  vk::BufferUsageFlagBits::eIndexBuffer,
														  mesh->GetIndices().data());
	}
}

struct VulkanMesh::Internal {
	const RPG::VulkanBuffer vertexBuffer;
	const RPG::VulkanBuffer indexBuffer;
	const uint32_t numIndices;

	Internal(const RPG::VulkanPhysicalDevice& physicalDevice,
			 const RPG::VulkanDevice& device,
			 const RPG::VulkanCommandPool& commandPool,
			 std::shared_ptr<RPG::Mesh> mesh)
			: vertexBuffer(::CreateVertexBuffer(physicalDevice, device, commandPool, mesh)),
			  indexBuffer(::CreateIndexBuffer(physicalDevice, device, commandPool, mesh)),
			  numIndices(mesh->GetNumIndices()) {
	}
};

VulkanMesh::VulkanMesh(const RPG::VulkanPhysicalDevice& physicalDevice,
					   const RPG::VulkanDevice& device,
					   const RPG::VulkanCommandPool& commandPool,
					   std::shared_ptr<RPG::Mesh> mesh)
		: internal(RPG::MakeInternalPointer<Internal>(physicalDevice, device, commandPool, mesh)) {
}

const vk::Buffer& VulkanMesh::GetVertexBuffer() const {
	return internal->vertexBuffer.GetBuffer();
}

const vk::Buffer& VulkanMesh::GetIndexBuffer() const {
	return internal->indexBuffer.GetBuffer();
}

const uint32_t& VulkanMesh::GetNumIndices() const {
	return internal->numIndices;
}