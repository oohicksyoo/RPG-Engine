//
// Created by Alex on 12/14/2020.
//

#include "VulkanSurface.hpp"

using RPG::VulkanSurface;

namespace {
	vk::UniqueSurfaceKHR CreateSurface(const vk::Instance& instance, const RPG::VulkanPhysicalDevice& physicalDevice, const RPG::SDLWindow& window) {
		static const std::string logTag{ "RPG::VulkanSurface::CreateSurface" };

		VkSurfaceKHR sdlSurface;

		// Ask SDL to create a Vulkan surface from its window.
		if (!SDL_Vulkan_CreateSurface(window.GetWindow(), instance, &sdlSurface)) {
			throw std::runtime_error("[" + logTag + "] SDL could not create a Vulkan surface.");
		}

		// Wrap the result in a Vulkan managed surface object.
		return vk::UniqueSurfaceKHR{ sdlSurface, instance };
	}
}

struct VulkanSurface::Internal {
	const vk::UniqueSurfaceKHR surface;

	Internal(const vk::Instance& instance, const RPG::VulkanPhysicalDevice& physicalDevice, const RPG::SDLWindow& window)
			: surface(::CreateSurface(instance, physicalDevice, window)) {
	}
};

VulkanSurface::VulkanSurface(const vk::Instance& instance, const RPG::VulkanPhysicalDevice& physicalDevice, const RPG::SDLWindow& window)
		: internal(RPG::MakeInternalPointer<Internal>(instance, physicalDevice, window)) {
}

const vk::SurfaceKHR& VulkanSurface::GetSurface() const {
	return *internal->surface;
}