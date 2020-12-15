//
// Created by Alex on 12/14/2020.
//

#include "VulkanCommon.hpp"
#include "../../core/GraphicsWrapper.hpp"
#include "../../core/Log.hpp"
#include "../../core/SDLWrapper.hpp"
#include <set>

std::vector<std::string> RPG::Vulkan::GetRequiredVulkanExtensionNames() {
	uint32_t extensionCount;
	SDL_Vulkan_GetInstanceExtensions(nullptr, &extensionCount, nullptr);

	auto extensionNames{std::make_unique<const char**>(new const char*[extensionCount])};
	SDL_Vulkan_GetInstanceExtensions(nullptr, &extensionCount, *extensionNames);
	std::vector<std::string> result(*extensionNames, *extensionNames + extensionCount);

	return result;
}

bool RPG::Vulkan::IsVulkanAvailable() {
	static const std::string logTag{"RPG::Vulkan::IsVulkanAvailable"};

	#ifdef __ANDROID__
		//Try to dynamically load the vulkan library and seed the function pointer mapping
		if (!InitVulkan()) {
			return false;
		}
	#endif

	// Check if SDL itself can load Vulkan.
	if (SDL_Vulkan_LoadLibrary(nullptr) != 0) 	{
		RPG::Log(logTag, "No SDL Vulkan support found.");
		return false;
	}

	// Determine what Vulkan extensions are required by SDL to be able to run Vulkan then pump
	// them into a 'set' so we can evaluate them easily.
	std::vector<std::string> requiredExtensionNamesSource {RPG::Vulkan::GetRequiredVulkanExtensionNames()};

	std::set<std::string> requiredExtensionNames (requiredExtensionNamesSource.begin(), requiredExtensionNamesSource.end());

	// There should always be required extensions so we should never get 0.
	if (requiredExtensionNames.empty()) {
		RPG::Log(logTag, "No Vulkan required extensions found.");
		return false;
	}

	// Iterate all the available Vulkan extensions on the current device, draining
	// each one from the required extensions set along the way.
	for (auto const& availableExtension : vk::enumerateInstanceExtensionProperties()) {
		requiredExtensionNames.erase(availableExtension.extensionName);
	}

	// If our required extensions set isn't empty it means that one or more of
	// them were not found in the available extensions, so we don't have what
	// we require to successfully create a Vulkan instance.
	if (!requiredExtensionNames.empty()) {
		RPG::Log(logTag, "Missing one or more required Vulkan extensions.");
		return false;
	}

	RPG::Log(logTag, "Vulkan is available.");
	return true;
}