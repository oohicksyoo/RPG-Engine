//
// Created by Alex on 12/14/2020.
//

#include "VulkanAssetManager.hpp"
#include "../../core/Assets.hpp"
#include "../../core/Log.hpp"
#include <unordered_map>

using RPG::VulkanAssetManager;

namespace {
	RPG::VulkanPipeline CreatePipeline(const RPG::Assets::Pipeline& pipeline,
									   const RPG::VulkanPhysicalDevice& physicalDevice,
									   const RPG::VulkanDevice& device,
									   const RPG::VulkanRenderContext& renderContext) {
		const std::string pipelinePath{RPG::Assets::ResolvePipelinePath(pipeline)};

		RPG::Log("RPG::VulkanAssetManager::CreatePipeline", "Creating pipeline: " + pipelinePath);

		return RPG::VulkanPipeline(physicalDevice,
								   device,
								   pipelinePath,
								   renderContext.GetViewport(),
								   renderContext.GetScissor(),
								   renderContext.GetRenderPass());
	}

	RPG::VulkanMesh CreateMesh(const RPG::VulkanPhysicalDevice& physicalDevice,
							   const RPG::VulkanDevice& device,
							   const RPG::VulkanCommandPool& commandPool,
							   const RPG::Assets::StaticMesh& staticMesh) {
		std::string meshPath{RPG::Assets::ResolveStaticMeshPath(staticMesh)};

		RPG::Log("RPG::VulkanAssetManager::CreateMesh", "Creating static mesh from " + meshPath);

		return RPG::VulkanMesh(physicalDevice,
							   device,
							   commandPool,
							   RPG::Assets::LoadOBJFile(meshPath));
	}

	RPG::VulkanTexture CreateTexture(const RPG::Assets::Texture& texture,
									 const RPG::VulkanPhysicalDevice& physicalDevice,
									 const RPG::VulkanDevice& device,
									 const RPG::VulkanCommandPool& commandPool,
									 const RPG::VulkanRenderContext& renderContext) {
		std::string texturePath{RPG::Assets::ResolveTexturePath(texture)};

		RPG::Log("RPG::VulkanAssetManager::CreateTexture", "Creating texture from " + texturePath);

		RPG::Bitmap bitmap{RPG::Assets::LoadBitmapFile(texturePath)};

		return RPG::VulkanTexture(texture,
								  physicalDevice,
								  device,
								  commandPool,
								  bitmap);
	}
}

struct VulkanAssetManager::Internal {
	std::unordered_map<RPG::Assets::Pipeline, RPG::VulkanPipeline> pipelineCache;
	std::unordered_map<RPG::Assets::StaticMesh, RPG::VulkanMesh> staticMeshCache;
	std::unordered_map<RPG::Assets::Texture, RPG::VulkanTexture> textureCache;

	Internal() {}

	void LoadAssetManifest(const RPG::VulkanPhysicalDevice& physicalDevice,
						   const RPG::VulkanDevice& device,
						   const RPG::VulkanRenderContext& renderContext,
						   const RPG::VulkanCommandPool& commandPool,
						   const RPG::AssetManifest& assetManifest) {
		for (const auto& pipeline : assetManifest.pipelines) {
			if (pipelineCache.count(pipeline) == 0) {
				pipelineCache.insert(std::make_pair(pipeline, ::CreatePipeline(pipeline, physicalDevice, device, renderContext)));
			}
		}

		for (const auto& staticMesh : assetManifest.staticMeshes) {
			if (staticMeshCache.count(staticMesh) == 0) {
				staticMeshCache.insert(std::make_pair(staticMesh, ::CreateMesh(physicalDevice, device, commandPool, staticMesh)));
			}
		}

		for (const auto& texture : assetManifest.textures) {
			if (textureCache.count(texture) == 0) {
				textureCache.insert(std::make_pair(texture, ::CreateTexture(texture, physicalDevice, device, commandPool, renderContext)));
			}
		}
	}

	void ReloadContextualAssets(const RPG::VulkanPhysicalDevice& physicalDevice,
								const RPG::VulkanDevice& device,
								const RPG::VulkanRenderContext& renderContext) {
		for (auto& element : pipelineCache) {
			element.second = ::CreatePipeline(element.first, physicalDevice, device, renderContext);
		}
	}
};

VulkanAssetManager::VulkanAssetManager() : internal(RPG::MakeInternalPointer<Internal>()) {}

void VulkanAssetManager::LoadAssetManifest(const RPG::VulkanPhysicalDevice& physicalDevice,
										   const RPG::VulkanDevice& device,
										   const RPG::VulkanRenderContext& renderContext,
										   const RPG::VulkanCommandPool& commandPool,
										   const RPG::AssetManifest& assetManifest) {
	internal->LoadAssetManifest(physicalDevice, device, renderContext, commandPool, assetManifest);
}

void VulkanAssetManager::ReloadContextualAssets(const RPG::VulkanPhysicalDevice& physicalDevice,
												const RPG::VulkanDevice& device,
												const RPG::VulkanRenderContext& renderContext) {
	internal->ReloadContextualAssets(physicalDevice, device, renderContext);
}

const RPG::VulkanPipeline& VulkanAssetManager::GetPipeline(const RPG::Assets::Pipeline& pipeline) const {
	return internal->pipelineCache.at(pipeline);
}

const RPG::VulkanMesh& VulkanAssetManager::GetStaticMesh(const RPG::Assets::StaticMesh& staticMesh) const {
	return internal->staticMeshCache.at(staticMesh);
}

const RPG::VulkanTexture& VulkanAssetManager::GetTexture(const RPG::Assets::Texture& texture) const {
	return internal->textureCache.at(texture);
}