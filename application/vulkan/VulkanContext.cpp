//
// Created by Alex on 12/14/2020.
//

#include "VulkanContext.hpp"
#include "../../core/GraphicsWrapper.hpp"
#include "../../core/Log.hpp"
#include "../../core/SDLWindow.hpp"
#include "VulkanCommon.hpp"
#include "VulkanPhysicalDevice.hpp"
#include "VulkanSurface.hpp"
#include "VulkanDevice.hpp"
#include "VulkanRenderContext.hpp"
#include "VulkanCommandPool.hpp"
#include "VulkanAssetManager.hpp"
#include <set>
#include <vector>

using RPG::VulkanContext;

namespace {
	std::vector<std::string> GetDesiredValidationLayers() {
		std::vector<std::string> result;

		#ifndef NDEBUG
			// If we are in a debug build we will cultivate a list of validation layers.
			static const std::string logTag{"RPG::VulkanContext::GetDesiredValidationLayers"};

			// Collate which validations layers we are interested in applying if they are available.
			std::set<std::string> desiredLayers{"VK_LAYER_LUNARG_standard_validation"};

			// Iterate all the available layers for the current device.
			for (auto const& properties : vk::enumerateInstanceLayerProperties()) {
				std::string layerName{properties.layerName};

				RPG::Log(logTag, "Available layer: " + layerName);

				// If we are interested in this layer, add it to the result list.
				if (desiredLayers.count(layerName)) {
					RPG::Log(logTag, "*** Found desired layer: " + layerName);
					result.push_back(layerName);
				}
			}
		#endif

		return result;
	}

	vk::UniqueInstance CreateInstance() {
		//TODO: VULKAN: Grab application name here
		vk::ApplicationInfo applicationInfo {
				"RPG Engine",      // Application name
				VK_MAKE_VERSION(1, 0, 0), // Application version
				"RPG Engine",      // Engine name
				VK_MAKE_VERSION(1, 0, 0), // Engine version
				VK_MAKE_VERSION(1, 0, 0)  // Vulkan API version
		};

		// Find out what the mandatory Vulkan extensions are on the current device,
		// by this stage we would have already determined that the extensions are
		// available via the 'rpg::vulkan::isVulkanAvailable()' call in our main engine.
		std::vector<std::string> requiredExtensionNames {RPG::Vulkan::GetRequiredVulkanExtensionNames()};

		// Pack the extension names into a data format consumable by Vulkan.
		std::vector<const char*> extensionNames;
		for (const auto& extension : requiredExtensionNames) {
			extensionNames.push_back(extension.c_str());
		}

		//Determine what validation layers can and should be activated
		std::vector<std::string> desiredValidationLayers{::GetDesiredValidationLayers()};

		//Pack the validation layers into a data format consumable by vulkan
		std::vector<const char*> validationLayers;
		for (const auto& layer : desiredValidationLayers) {
			validationLayers.push_back(layer.c_str());
		}

		// Define the info for creating our Vulkan instance.
		vk::InstanceCreateInfo instanceCreateInfo {
				vk::InstanceCreateFlags(),                    // Flags
				&applicationInfo,                             // Application info
				static_cast<uint32_t>(validationLayers.size()),// Enabled layer count
				validationLayers.data(),                      // Enabled layer names
				static_cast<uint32_t>(extensionNames.size()), // Enabled extension count
				extensionNames.data()                         // Enabled extension names
		};

		// Build a new Vulkan instance from the configuration.
		return vk::createInstanceUnique(instanceCreateInfo);
	}
}

struct VulkanContext::Internal {
	const vk::UniqueInstance instance;
	const RPG::VulkanPhysicalDevice physicalDevice;
	const RPG::SDLWindow window;
	const RPG::VulkanSurface surface;
	const RPG::VulkanDevice device;
	const RPG::VulkanCommandPool commandPool;
	RPG::VulkanRenderContext renderContext;
	RPG::VulkanAssetManager assetManager;

	Internal() : instance(::CreateInstance()),
				 physicalDevice(RPG::VulkanPhysicalDevice(*instance)),
				 window(RPG::SDLWindow(SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI)),
				 surface(RPG::VulkanSurface(*instance, physicalDevice, window)),
				 device(RPG::VulkanDevice(physicalDevice, surface)),
				 commandPool(RPG::VulkanCommandPool(device)),
				 renderContext(RPG::VulkanRenderContext(window, physicalDevice, device, surface, commandPool)),
				 assetManager(RPG::VulkanAssetManager(physicalDevice, device, renderContext, commandPool)){
		RPG::Log("RPG::VulkanContext", "Initialized Vulkan context successfully.");
	}

	void LoadAssetManifest(const RPG::AssetManifest& assetManifest) {
		assetManager.LoadAssetManifest(physicalDevice, device, renderContext, commandPool, assetManifest);
	}

	void RecreateRenderContext() {
		device.GetDevice().waitIdle();
		renderContext = renderContext.Recreate(window, physicalDevice, device, surface, commandPool);
		assetManager.ReloadContextualAssets(physicalDevice, device, renderContext);
	}

	bool RenderBegin() {
		if (!renderContext.RenderBegin(device)) {
			RecreateRenderContext();
			return false;
		}

		return true;
	}

	//Removed in favour of switching to Framebuffers
	/*void Render(const RPG::Assets::Pipeline& pipeline, const std::shared_ptr<RPG::Hierarchy> hierarchy, const glm::mat4 cameraMatrix, const uint32_t shadowMap) {
		assetManager.GetPipeline(pipeline).Render(device, renderContext.GetActiveCommandBuffer(), assetManager, hierarchy, cameraMatrix, shadowMap);
	}*/

	void RenderEnd() {
		if (!renderContext.RenderEnd(device)) {
			RecreateRenderContext();
		}
	}

	glm::vec2 GetWindowPosition() {
		int windowX, windowY;
		SDL_GetWindowPosition(window.GetWindow(), &windowX, &windowY);
		return {windowX, windowY};
	}
};

VulkanContext::VulkanContext() : internal(RPG::MakeInternalPointer<Internal>()) {}

void VulkanContext::LoadAssetManifest(const RPG::AssetManifest& assetManifest) {
	internal->LoadAssetManifest(assetManifest);
}

bool VulkanContext::RenderBegin() {
	return internal->RenderBegin();
}

void VulkanContext::RenderToFrameBuffer(const RPG::Assets::Pipeline &pipeline,
										const std::shared_ptr<RPG::Hierarchy> hierarchy,
										const std::shared_ptr<RPG::FrameBuffer> framebuffer,
										const glm::mat4 cameraMatrix,
										const glm::vec3 clearColor,
                                        const uint32_t shadowMap,
										const bool isGameCamera) {
	//TODO: Vulkan Framebuffer setup
}

void VulkanContext::RenderLinesToFrameBuffer(const RPG::Assets::Pipeline &pipeline,
											 const std::shared_ptr<RPG::FrameBuffer> framebuffer,
											 const glm::mat4 cameraMatrix) {
	//TODO: Vulkan render lines to framebuffer
}

void VulkanContext::RenderToDepthBuffer(const RPG::Assets::Pipeline &pipeline,
                                        const std::shared_ptr<RPG::Hierarchy> hierarchy,
                                        const std::shared_ptr<RPG::FrameBuffer> framebuffer) {
    //TODO: Vulkan framebuffer for depth
}

void VulkanContext::RenderEnd() {
	internal->RenderEnd();
}

RPG::WindowSize VulkanContext::GetCurrentWindowSize() const {
	return RPG::SDL::GetWindowSize(internal->window.GetWindow());
}

glm::vec2 VulkanContext::GetWindowPosition() {
	return internal->GetWindowPosition();
}

void VulkanContext::ClearFrameBufferToColor(const RPG::Assets::Pipeline &pipeline,
                                            const std::shared_ptr<RPG::FrameBuffer> framebuffer, const glm::vec3 clearColor) {
    //TODO: Implement for Vulkan
}

void VulkanContext::RenderToFrameBuffer(const RPG::Assets::Pipeline &pipeline,
                                        const std::shared_ptr<RPG::FrameBuffer> framebuffer,
                                        const std::vector<RPG::GameObjectMaterialGroup> gameObjects,
                                        const glm::mat4 cameraMatrix) {
    //TODO: Implement for Vulkan
}

void VulkanContext::DisplayFrameBuffer(const RPG::Assets::Pipeline &pipeline, const std::shared_ptr<RPG::FrameBuffer> frameBuffer) {
    //TODO: Implement release implementation of displaying the scene to screen
}