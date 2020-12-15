//
// Created by Alex on 12/14/2020.
//

#include "VulkanPhysicalDevice.hpp"
#include "../../core/Log.hpp"
#include <vector>
#include <stack>

using RPG::VulkanPhysicalDevice;

namespace {
	vk::PhysicalDevice CreatePhysicalDevice(const vk::Instance& instance) {
		static const std::string logTag{"RPG::VulkanPhysicalDevice::CreatePhysicalDevice"};

		// Ask Vulkan for all the available physical devices in the current instance.
		std::vector<vk::PhysicalDevice> devices{instance.enumeratePhysicalDevices()};

		// If there are no physical devices available that can drive Vulkan then we
		// will deliberately throw an exception which will ultimately cause our
		// application to fall back to the OpenGL implementation.
		if (devices.empty()) {
			throw std::runtime_error("[" + logTag + "] No Vulkan physical devices found.");
		}

		// Pick the first device as the default which will be used if no better devices are found.
		vk::PhysicalDevice selectedDevice{devices[0]};

		// Grab the properties of the first device so we can query them for capabilities.
		vk::PhysicalDeviceProperties selectedProperties{selectedDevice.getProperties()};

		// If the device we have selected by default does not have a discrete GPU, then we will
		// try to search through any other physical devices looking for one with a discrete GPU.
		// Some computers will have an 'integrated' GPU as well as a 'discrete' GPU, where the
		// integrated GPU is typically not designed for high end graphics tasks, whereas a discrete
		// GPU is exactly for that purpose.
		if (selectedProperties.deviceType != vk::PhysicalDeviceType::eDiscreteGpu) {
			for (size_t i = 1; i < devices.size(); i++) {
				vk::PhysicalDevice& nextDevice{devices[i]};
				vk::PhysicalDeviceProperties nextProperties{nextDevice.getProperties()};

				// If we find a device with a discrete GPU, then choose it and we are done.
				if (nextProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
					selectedDevice = nextDevice;
					selectedProperties = nextProperties;
					break;
				}
			}
		}

		// Next we need to make sure that the physical device can support a swapchain.
		bool hasSwapchainSupport{false};
		std::string swapchainName{VK_KHR_SWAPCHAIN_EXTENSION_NAME};

		// Traverse all the extensions available in the physical device, looking for the
		// presence of the swapchain extension.
		for (const auto& extension : selectedDevice.enumerateDeviceExtensionProperties()) {
			if (extension.extensionName == swapchainName) {
				hasSwapchainSupport = true;
				break;
			}
		}

		// We can't render without swapchain support.
		if (!hasSwapchainSupport) {
			throw std::runtime_error(logTag + ": Swapchain support not found.");
		}

		// We should now have selected a physical device, which may or may not have a discrete GPU
		// but will have been selected with a preference to having one.
		RPG::Log(logTag, "Physical device: " + std::string{selectedProperties.deviceName} +".");

		return selectedDevice;
	}

	vk::SampleCountFlagBits GetMultiSamplingLevel(const vk::PhysicalDevice& physicalDevice) {
		static const std::string logTag{ "RPG::VulkanPhysicalDevice::GetMultiSamplingLevel" };

		vk::PhysicalDeviceProperties properties{physicalDevice.getProperties()};
		vk::SampleCountFlags supportedSampleCounts{properties.limits.framebufferColorSampleCounts};

		std::stack<vk::SampleCountFlagBits> preferredSampleCounts;
		preferredSampleCounts.push(vk::SampleCountFlagBits::e1);
		preferredSampleCounts.push(vk::SampleCountFlagBits::e2);
		preferredSampleCounts.push(vk::SampleCountFlagBits::e4);
		preferredSampleCounts.push(vk::SampleCountFlagBits::e8);

		while (!preferredSampleCounts.empty()) {
			// Take the sample count at the top of the stack and see if it is supported.
			vk::SampleCountFlagBits sampleCount{preferredSampleCounts.top()};

			if (supportedSampleCounts & sampleCount) {
				return sampleCount;
			}

			// If our preferred sample count is not found, pop the stack ready for the next iteration.
			preferredSampleCounts.pop();
		}

		// If none of our sample counts is found, multi sampling is not supported on this device ...
		throw std::runtime_error("[" + logTag + "] Multi sampling not supported.");
	}

	vk::Format GetDepthFormat(const vk::PhysicalDevice& physicalDevice) {
		static const std::string logTag{ "RPG::VulkanPhysicalDevice::GetDepthFormat" };

		vk::FormatProperties formatProperties{physicalDevice.getFormatProperties(vk::Format::eD32Sfloat)};

		if (formatProperties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eDepthStencilAttachment) {
			return vk::Format::eD32Sfloat;
		}

		throw std::runtime_error("[" + logTag + "] 32 bit signed depth stencil format not supported.");
	}

	uint32_t GetMemoryTypeIndex(const vk::PhysicalDevice& physicalDevice,
								const uint32_t& filter,
								const vk::MemoryPropertyFlags& flags) {
		// Fetch all the memory properties of the physical device.
		vk::PhysicalDeviceMemoryProperties memoryProperties{physicalDevice.getMemoryProperties()};

		// Loop through each of the memory type fields in the properties.
		for (uint32_t index = 0; index < memoryProperties.memoryTypeCount; index++) {
			// If the current memory type is available and has all the property flags required, we
			// have found a position in the physical device memory indices that is compatible.
			if ((filter & (1 << index)) && (memoryProperties.memoryTypes[index].propertyFlags & flags) == flags) {
				return index;
			}
		}

		// If no memory type could be found that meets our criteria we can't proceed.
		throw std::runtime_error("ast::VulkanImage::getMemoryTypeIndex: Failed to find suitable memory type.");
	}

	bool GetShaderMultiSamplingSupport(const vk::PhysicalDevice& physicalDevice) {
		return physicalDevice.getFeatures().sampleRateShading;
	}

	bool GetAnisotropicFilteringSupport(const vk::PhysicalDevice& physicalDevice) {
		return physicalDevice.getFeatures().samplerAnisotropy;
	}
}

struct VulkanPhysicalDevice::Internal {
	const vk::PhysicalDevice physicalDevice;
	const vk::SampleCountFlagBits multiSamplingLevel;
	const vk::Format depthFormat;
	const bool shaderMultiSamplingSupported;
	const bool anisotropicFilteringSupported;

	Internal(const vk::Instance& instance) : physicalDevice(::CreatePhysicalDevice(instance)),
											 multiSamplingLevel(::GetMultiSamplingLevel(physicalDevice)),
											 depthFormat(::GetDepthFormat(physicalDevice)),
											 shaderMultiSamplingSupported(::GetShaderMultiSamplingSupport(physicalDevice)),
											 anisotropicFilteringSupported(::GetAnisotropicFilteringSupport(physicalDevice)) {
	}
};

VulkanPhysicalDevice::VulkanPhysicalDevice(const vk::Instance& instance) : internal(RPG::MakeInternalPointer<Internal>(instance)) {}

const vk::PhysicalDevice& VulkanPhysicalDevice::GetPhysicalDevice() const {
	return internal->physicalDevice;
}

const vk::Format VulkanPhysicalDevice::GetDepthFormat() const {
	return internal->depthFormat;
}

const vk::SampleCountFlagBits VulkanPhysicalDevice::GetMultiSamplingLevel() const {
	return internal->multiSamplingLevel;
}

uint32_t VulkanPhysicalDevice::GetMemoryTypeIndex(const uint32_t& filter, const vk::MemoryPropertyFlags& flags) const {
	return ::GetMemoryTypeIndex(internal->physicalDevice, filter, flags);
}

bool VulkanPhysicalDevice::IsShaderMultiSamplingSupported() const {
	return internal->shaderMultiSamplingSupported;
}

bool VulkanPhysicalDevice::IsAnisotropicFilteringSupported() const {
	return internal->anisotropicFilteringSupported;
}