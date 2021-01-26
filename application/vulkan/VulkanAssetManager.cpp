//
// Created by Alex on 12/14/2020.
//

#include "VulkanAssetManager.hpp"
#include "../../core/Assets.hpp"
#include "../../core/Log.hpp"
#include "../../core/Content.hpp"
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
							   std::shared_ptr<RPG::Mesh> mesh) {

		RPG::Log("RPG::VulkanAssetManager::CreateMesh", "Creating static mesh");

		return RPG::VulkanMesh(physicalDevice,
							   device,
							   commandPool,
							   mesh);
	}

	RPG::VulkanTexture CreateTexture(std::shared_ptr<RPG::Bitmap> bitmap,
									 std::string bitmapPath,
									 const RPG::VulkanPhysicalDevice& physicalDevice,
									 const RPG::VulkanDevice& device,
									 const RPG::VulkanCommandPool& commandPool,
									 const RPG::VulkanRenderContext& renderContext) {
		RPG::Log("RPG::VulkanAssetManager::CreateTexture", "Creating texture");

		return RPG::VulkanTexture(bitmapPath,
								  physicalDevice,
								  device,
								  commandPool,
								  bitmap);
	}
}

struct VulkanAssetManager::Internal {
	const RPG::VulkanPhysicalDevice& physicalDevice;
	const RPG::VulkanDevice& device;
	const RPG::VulkanRenderContext& renderContext;
	const RPG::VulkanCommandPool& commandPool;

	std::unordered_map<RPG::Assets::Pipeline, RPG::VulkanPipeline> pipelineCache;
	std::unordered_map<std::string, RPG::VulkanMesh> staticMeshCache;
	std::unordered_map<std::string, RPG::VulkanTexture> textureCache;

	Internal(const RPG::VulkanPhysicalDevice& physicalDevice,
			 const RPG::VulkanDevice& device,
			 const RPG::VulkanRenderContext& renderContext,
			 const RPG::VulkanCommandPool& commandPool) : physicalDevice(physicalDevice),
			 device(device),
			 renderContext(renderContext),
			 commandPool(commandPool) {

		RPG::Content::GetInstance().OnLoadedAsset<RPG::Mesh>([this](std::string path, std::shared_ptr<RPG::Mesh> mesh) {
			if (staticMeshCache.count(path) == 0) {
				RPG::Log("AssetManager", "Adding new mesh to cache (" + path + ")");
				staticMeshCache.insert(std::make_pair(path, ::CreateMesh(this->physicalDevice, this->device, this->commandPool, mesh)));
			}
		});

		RPG::Content::GetInstance().OnLoadedAsset<RPG::Bitmap>([this](std::string path, std::shared_ptr<RPG::Bitmap> bitmap) {
			if (textureCache.count(path) == 0) {
				RPG::Log("AssetManager", "Adding new bitmap to cache (" + path + ")");
				textureCache.insert(std::make_pair(path, ::CreateTexture(bitmap, path, this->physicalDevice, this->device, this->commandPool, this->renderContext)));
			}
		});
	}

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
	}

	void ReloadContextualAssets(const RPG::VulkanPhysicalDevice& physicalDevice,
								const RPG::VulkanDevice& device,
								const RPG::VulkanRenderContext& renderContext) {
		for (auto& element : pipelineCache) {
			element.second = ::CreatePipeline(element.first, physicalDevice, device, renderContext);
		}
	}
};

VulkanAssetManager::VulkanAssetManager(const RPG::VulkanPhysicalDevice& physicalDevice,
									   const RPG::VulkanDevice& device,
									   const RPG::VulkanRenderContext& renderContext,
									   const RPG::VulkanCommandPool& commandPool) : internal(RPG::MakeInternalPointer<Internal>(physicalDevice, device, renderContext, commandPool)) {}

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

const RPG::VulkanMesh& VulkanAssetManager::GetStaticMesh(std::string staticMesh) const {
	return internal->staticMeshCache.at(staticMesh);
}

const RPG::VulkanTexture& VulkanAssetManager::GetTexture(std::string texture) const {
	return internal->textureCache.at(texture);
}