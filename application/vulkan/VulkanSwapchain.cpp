//
// Created by Alex on 12/14/2020.
//

#include "VulkanSwapchain.hpp"
#include "../../core/Log.hpp"
#include <stack>

using RPG::VulkanSwapchain;

namespace {
	struct VulkanSwapchainFormat {
		vk::ColorSpaceKHR colorSpace;
		vk::Format colorFormat;
	};

	VulkanSwapchainFormat GetFormat(const RPG::VulkanPhysicalDevice& physicalDevice,
									const RPG::VulkanSurface& surface) {
		static const std::string logTag{ "RPG::VulkanSwapchain::GetFormat" };

		// We need to make sure that there is at least one surface format compatible with our surface.
		std::vector<vk::SurfaceFormatKHR> availableSurfaceFormats{
				physicalDevice.GetPhysicalDevice().getSurfaceFormatsKHR(surface.GetSurface()) };

		size_t availableFormatCount{ availableSurfaceFormats.size() };

		if (availableFormatCount == 0) {
			throw std::runtime_error(logTag + ": No compatible surface formats found.");
		}

		// Take the first format as a 'default'.
		vk::SurfaceFormatKHR defaultFormat{ availableSurfaceFormats[0] };

		// If there is only one surface with an undefined format, we will manually choose one.
		if (availableFormatCount == 1 && defaultFormat.format == vk::Format::eUndefined) {
			RPG::Log(logTag, "Surface format is undefined: defaulting to eSrgbNonlinear + eR8G8B8Unorm.");
			return VulkanSwapchainFormat{ vk::ColorSpaceKHR::eSrgbNonlinear, vk::Format::eR8G8B8Unorm };
		}

		// We will look through the available formats, attempting to prefer the eR8G8B8Unorm type.
		for (const auto& availableFormat : availableSurfaceFormats) {
			if (availableFormat.format == vk::Format::eR8G8B8Unorm) {
				RPG::Log(logTag, "Found supported eR8G8B8Unorm surface format.");
				return VulkanSwapchainFormat{ availableFormat.colorSpace, availableFormat.format };
			}
		}

		// Otherwise we will just have to use the first available format.
		RPG::Log(logTag, "Surface format eR8G8B8Unorm not found, using default available format.");
		return VulkanSwapchainFormat{ defaultFormat.colorSpace, defaultFormat.format };
	}

	vk::PresentModeKHR GetPresentationMode(const RPG::VulkanPhysicalDevice& physicalDevice,
										   const RPG::VulkanSurface& surface) {
		static const std::string logTag{ "RPG::VulkanSwapchain::GetPresentationMode" };

		// We need to make sure that there is at least one presentation mode compatible with our surface.
		std::vector<vk::PresentModeKHR> availableModes{
				physicalDevice.GetPhysicalDevice().getSurfacePresentModesKHR(surface.GetSurface()) };

		if (availableModes.empty()) {
			throw std::runtime_error(logTag + ": No compatible present modes found.");
		}

		// Load up the presentation modes into a stack so they are popped
		// in our preferred order for evaluation.
		std::stack<vk::PresentModeKHR> preferredModes;
		preferredModes.push(vk::PresentModeKHR::eImmediate);
		preferredModes.push(vk::PresentModeKHR::eFifoRelaxed);
		preferredModes.push(vk::PresentModeKHR::eFifo);
		preferredModes.push(vk::PresentModeKHR::eMailbox);

		while (!preferredModes.empty()) {
			// Take the mode at the top of the stack and see if the list of available modes contains it.
			vk::PresentModeKHR mode{ preferredModes.top() };

			if (std::find(availableModes.begin(), availableModes.end(), mode) != availableModes.end()) {
				// If we find the current preferred presentation mode, we are done.
				return mode;
			}

			// If our preferred mode is not found, pop the stack ready for the next iteration.
			preferredModes.pop();
		}

		// None of our preferred presentation modes were found, can't go further...
		throw std::runtime_error(logTag + ": No compatible presentation modes found.");
	}

	vk::Extent2D GetExtent(const RPG::SDLWindow& window) {
		int drawableWidth;
		int drawableHeight;
		SDL_Vulkan_GetDrawableSize(window.GetWindow(), &drawableWidth, &drawableHeight);

		return vk::Extent2D{
				static_cast<uint32_t>(drawableWidth),
				static_cast<uint32_t>(drawableHeight)
		};
	}

	vk::UniqueSwapchainKHR CreateSwapchain(
			const RPG::VulkanPhysicalDevice& physicalDevice,
			const RPG::VulkanDevice& device,
			const RPG::VulkanSurface& surface,
			const VulkanSwapchainFormat& format,
			const vk::PresentModeKHR& presentationMode,
			const vk::Extent2D& extent,
			const vk::SurfaceTransformFlagBitsKHR& transform,
			const vk::SwapchainKHR& oldSwapchain) {
		// Grab the capabilities of the current physical device in relation to the surface.
		vk::SurfaceCapabilitiesKHR surfaceCapabilities{
				physicalDevice.GetPhysicalDevice().getSurfaceCapabilitiesKHR(surface.GetSurface()) };

		// We will pick a minimum image count of +1 to the minimum supported on the device.
		uint32_t minimumImageCount{ surfaceCapabilities.minImageCount + 1 };
		uint32_t maxImageCount{ surfaceCapabilities.maxImageCount };

		// Make sure our image count doesn't exceed any maximum if there is one.
		// Note: The Vulkan docs state that a value of 0 doesn't mean there is
		// a limit of 0, it means there there is no limit.
		if (maxImageCount > 0 && minimumImageCount > maxImageCount) {
			minimumImageCount = maxImageCount;
		}

		vk::SwapchainCreateInfoKHR createInfo{
				vk::SwapchainCreateFlagsKHR(),            // Flags
				surface.GetSurface(),                     // Surface
				minimumImageCount,                        // Minimum image count
				format.colorFormat,                       // Image format
				format.colorSpace,                        // Image color space
				extent,                                   // Image extent
				1,                                        // Image array layers
				vk::ImageUsageFlagBits::eColorAttachment, // Image usage
				vk::SharingMode::eExclusive,              // Image sharing mode
				0,                                        // Queue family index count
				nullptr,                                  // Queue family indices
				transform,                                // Pre transform
				vk::CompositeAlphaFlagBitsKHR::eOpaque,   // Composite alpha
				presentationMode,                         // Present mode
				VK_TRUE,                                  // Clipped
				oldSwapchain };                      // Old swapchain

		// If our device has a discrete presentation queue, we must specify
		// that swapchain images are permitted to be shared between both
		// the graphics and presentation queues.
		if (device.HasDiscretePresentationQueue()) {
			std::array<uint32_t, 2> queueIndices{
					device.GetGraphicsQueueIndex(),
					device.GetPresentationQueueIndex()
			};

			createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueIndices.data();
		}

		return device.GetDevice().createSwapchainKHRUnique(createInfo);
	}

	std::vector<RPG::VulkanImageView> CreateImageViews(const RPG::VulkanDevice& device,
													   const vk::SwapchainKHR& swapChain,
													   const VulkanSwapchainFormat& format) {
		std::vector<RPG::VulkanImageView> imageViews;

		// For each of the images in the swap chain, we need to create a new 'image view'.
		for (const vk::Image& image : device.GetDevice().getSwapchainImagesKHR(swapChain)) {
			RPG::VulkanImageView imageView{
					device.GetDevice(),
					image,
					format.colorFormat,
					vk::ImageAspectFlagBits::eColor,
					1
			};

			imageViews.push_back(std::move(imageView));
		}

		return imageViews;
	}
}

struct VulkanSwapchain::Internal {
	const VulkanSwapchainFormat format;
	const vk::PresentModeKHR presentationMode;
	const vk::Extent2D extent;
	const vk::SurfaceTransformFlagBitsKHR transform;
	const vk::UniqueSwapchainKHR swapchain;
	const std::vector<RPG::VulkanImageView> imageViews;

	Internal(const RPG::SDLWindow& window,
			 const RPG::VulkanPhysicalDevice& physicalDevice,
			 const RPG::VulkanDevice& device,
			 const RPG::VulkanSurface& surface,
			 const vk::SwapchainKHR& oldSwapchain)
			: format(::GetFormat(physicalDevice, surface)),
			  presentationMode(::GetPresentationMode(physicalDevice, surface)),
			  extent(::GetExtent(window)),
			  transform(vk::SurfaceTransformFlagBitsKHR::eIdentity),
			  swapchain(::CreateSwapchain(physicalDevice, device, surface, format, presentationMode, extent, transform, oldSwapchain)),
			  imageViews(::CreateImageViews(device, swapchain.get(), format)) {
	}
};

VulkanSwapchain::VulkanSwapchain(const RPG::SDLWindow& window,
								 const RPG::VulkanPhysicalDevice& physicalDevice,
								 const RPG::VulkanDevice& device,
								 const RPG::VulkanSurface& surface,
								 const vk::SwapchainKHR& oldSwapchain)
		: internal(RPG::MakeInternalPointer<Internal>(window, physicalDevice, device, surface, oldSwapchain)) {
}

const vk::SwapchainKHR& VulkanSwapchain::GetSwapchain() const {
	return internal->swapchain.get();
}

const std::vector<RPG::VulkanImageView>& VulkanSwapchain::GetImageViews() const {
	return internal->imageViews;
}

const vk::Format& VulkanSwapchain::GetColorFormat() const {
	return internal->format.colorFormat;
}

const vk::Extent2D& VulkanSwapchain::GetExtent() const {
	return internal->extent;
}

uint32_t VulkanSwapchain::GetImageCount() const {
	return static_cast<uint32_t>(internal->imageViews.size());
}