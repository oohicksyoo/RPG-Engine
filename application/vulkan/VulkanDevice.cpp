//
// Created by Alex on 12/14/2020.
//

#include "VulkanDevice.hpp"
#include <vector>
#include <cstddef>

using RPG::VulkanDevice;

namespace {
	struct QueueConfig {
		uint32_t graphicsQueueIndex;
		uint32_t presentationQueueIndex;
		bool hasDiscretePresentationQueue;
	};

	QueueConfig GetQueueConfig(const vk::PhysicalDevice& physicalDevice, const vk::SurfaceKHR& surface) {
		static const std::string logTag{"RPG::VulkanPhysicalDevice::GetGraphicsQueueFamilyIndex"};

		// Assign properties to track selection of graphics and presentation queues,
		// initially with an unset marker value.
		#undef max
		constexpr uint32_t unsetQueueIndex{std::numeric_limits<uint32_t>::max()};
		uint32_t graphicsQueueIndex{unsetQueueIndex};
		uint32_t presentationQueueIndex{unsetQueueIndex};

		// Fetch all the queue family properties supported by the physical device.
		std::vector<vk::QueueFamilyProperties> queueFamilies{physicalDevice.getQueueFamilyProperties()};

		// Search through the available queue families, looking for one that supports graphics.
		for (size_t i = 0; i < queueFamilies.size(); i++) {
			vk::QueueFamilyProperties properties{queueFamilies[i]};

			// If the current queue family has graphics capability we will evaluate it as a
			// candidate for both the graphics queue and the presentation queue.
			if (properties.queueCount > 0 && properties.queueFlags & vk::QueueFlagBits::eGraphics) {
				const uint32_t currentQueueIndex{static_cast<uint32_t>(i)};

				// If we haven't yet chosen an index for the graphics queue (because it is unset),
				// then remember this index as the graphics queue index.
				if (graphicsQueueIndex == unsetQueueIndex) {
					graphicsQueueIndex = currentQueueIndex;
				}

				// We now need to see if the queue index can also behave as a presentation queue and
				// if so, both the graphics and presentation queue indices will be the same, effectively
				// meaning that we will only need to create a single queue and use it for both purposes.
				if (physicalDevice.getSurfaceSupportKHR(currentQueueIndex, surface)) {
					graphicsQueueIndex = currentQueueIndex;
					presentationQueueIndex = currentQueueIndex;

					// Since we have now discovered a queue index that can do BOTH graphics and presentation
					// we won't bother looking any further in the loop.
					break;
				}
			}
		}

		// If we couldn't find any queues that can perform graphics operations we are out.
		if (graphicsQueueIndex == unsetQueueIndex) {
			throw std::runtime_error(logTag + ": Could not find a graphics queue.");
		}

		// If we found a graphics queue, but not one that could also do presentation then we
		// need to find a discrete presentation queue to use instead, meaning we will end
		// up with two queues to manage instead of one that can do both.
		if (presentationQueueIndex == unsetQueueIndex) {
			for (size_t i = 0; i < queueFamilies.size(); i++) {
				const uint32_t currentQueueIndex{static_cast<uint32_t>(i)};

				if (physicalDevice.getSurfaceSupportKHR(currentQueueIndex, surface)) {
					presentationQueueIndex = currentQueueIndex;
					break;
				}
			}
		}

		// Once more we will see if we have a presentation queue or not - this time if
		// we still do not have one then we are out again.
		if (presentationQueueIndex == unsetQueueIndex) {
			throw std::runtime_error(logTag + ": Could not find a presentation queue.");
		}

		// At this point we have valid graphics queue and presentation queue indices
		// so we will wrap them up in a result object and return it. Note that we
		// will also record whether the presentation queue is 'discrete' meaning that
		// it is a different queue to the graphics queue.
		return QueueConfig{graphicsQueueIndex, presentationQueueIndex, graphicsQueueIndex != presentationQueueIndex};
	}

	vk::UniqueDevice CreateDevice(const RPG::VulkanPhysicalDevice& physicalDevice, const QueueConfig& queueConfig) {
		const float deviceQueuePriority{1.0f};

		// Hold a list of queue creation objects to use in our logical device,
		// initialising it with the graphics queue configuration.
		std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos{
				vk::DeviceQueueCreateInfo{
						vk::DeviceQueueCreateFlags(),   // Flags
						queueConfig.graphicsQueueIndex, // Queue family index
						1,                              // Queue count
						&deviceQueuePriority            // Queue priority
				}
		};

		// Add a presentation queue if the presentation queue needs to be
		// discrete from the graphics queue.
		if (queueConfig.hasDiscretePresentationQueue) {
			queueCreateInfos.push_back(vk::DeviceQueueCreateInfo{
					vk::DeviceQueueCreateFlags(),       // Flags
					queueConfig.presentationQueueIndex, // Queue family index
					1,                                  // Queue count
					&deviceQueuePriority                // Queue priority
			});
		}

		// We also need to request the swapchain extension be activated as we will need to use a swapchain
		std::vector<const char*> extensionNames{VK_KHR_SWAPCHAIN_EXTENSION_NAME};

		// Specify which physical device features to expose in our logical device
		vk::PhysicalDeviceFeatures physicalDeviceFeatures;

		// If shader based multisampling is available we will activate it.
		if (physicalDevice.IsShaderMultiSamplingSupported()) {
			physicalDeviceFeatures.sampleRateShading = true;
		}

		// If anisotropic filtering is available we will activate it.
		if (physicalDevice.IsAnisotropicFilteringSupported()) {
			physicalDeviceFeatures.samplerAnisotropy = true;
		}

		// Take the queue and extension name configurations and form the device creation definition.
		vk::DeviceCreateInfo deviceCreateInfo{
				vk::DeviceCreateFlags(),                        // Flags
				static_cast<uint32_t>(queueCreateInfos.size()), // Queue create info list count
				queueCreateInfos.data(),                        // Queue create info list
				0,                                              // Enabled layer count
				nullptr,                                        // Enabled layer names
				static_cast<uint32_t>(extensionNames.size()),   // Enabled extension count
				extensionNames.data(),                          // Enabled extension names
				& physicalDeviceFeatures                         // Physical device features
		};

		// Create a logical device with all the configuration we collated.
		return physicalDevice.GetPhysicalDevice().createDeviceUnique(deviceCreateInfo);
	}

	vk::Queue GetQueue(const vk::Device& device, const uint32_t& queueIndex) {
		return device.getQueue(queueIndex, 0);
	}

	std::vector<vk::UniqueSemaphore> CreateSemaphores(const vk::Device& device, const uint32_t& count) {
		std::vector<vk::UniqueSemaphore> semaphores;
		vk::SemaphoreCreateInfo info;

		for (uint32_t i = 0; i < count; i++) {
			semaphores.push_back(device.createSemaphoreUnique(info));
		}

		return semaphores;
	}

	std::vector<vk::UniqueFence> CreateFences(const vk::Device& device, const uint32_t& count) {
		std::vector<vk::UniqueFence> fences;
		vk::FenceCreateInfo info{vk::FenceCreateFlagBits::eSignaled};

		for (uint32_t i = 0; i < count; i++) {
			fences.push_back(device.createFenceUnique(info));
		}

		return fences;
	}

	vk::UniqueShaderModule CreateShaderModule(const vk::Device& device, const std::vector<char>& shaderCode) {
		vk::ShaderModuleCreateInfo info{
				vk::ShaderModuleCreateFlags(),                         // Flags
				shaderCode.size(),                                     // Code size
				reinterpret_cast<const uint32_t*>(shaderCode.data()) }; // Code

		return device.createShaderModuleUnique(info);
	}
}

struct VulkanDevice::Internal {
	const QueueConfig queueConfig;
	const vk::UniqueDevice device;
	const vk::Queue graphicsQueue;
	const vk::Queue presentationQueue;

	Internal(const RPG::VulkanPhysicalDevice& physicalDevice, const RPG::VulkanSurface& surface)
			: queueConfig(::GetQueueConfig(physicalDevice.GetPhysicalDevice(), surface.GetSurface())),
			  device(::CreateDevice(physicalDevice, queueConfig)),
			  graphicsQueue(::GetQueue(device.get(), queueConfig.graphicsQueueIndex)),
			  presentationQueue(::GetQueue(device.get(), queueConfig.presentationQueueIndex)) {
	}

	~Internal() {
		// We need to wait for the device to become idle before allowing it to be destroyed.
		device->waitIdle();
	}
};

VulkanDevice::VulkanDevice(const RPG::VulkanPhysicalDevice& physicalDevice,
						   const RPG::VulkanSurface& surface)
		: internal(RPG::MakeInternalPointer<Internal>(physicalDevice, surface)) {
}

const vk::Device& VulkanDevice::GetDevice() const {
	return *internal->device;
}

uint32_t VulkanDevice::GetGraphicsQueueIndex() const {
	return internal->queueConfig.graphicsQueueIndex;
}

uint32_t VulkanDevice::GetPresentationQueueIndex() const {
	return internal->queueConfig.presentationQueueIndex;
}

bool VulkanDevice::HasDiscretePresentationQueue() const {
	return internal->queueConfig.hasDiscretePresentationQueue;
}

const vk::Queue& VulkanDevice::GetGraphicsQueue() const {
	return internal->graphicsQueue;
}

std::vector<vk::UniqueSemaphore> VulkanDevice::CreateSemaphores(const uint32_t& count) const {
	return ::CreateSemaphores(internal->device.get(), count);
}

std::vector<vk::UniqueFence> VulkanDevice::CreateFences(const uint32_t& count) const {
	return ::CreateFences(internal->device.get(), count);
}

const vk::Queue& VulkanDevice::GetPresentationQueue() const {
	return internal->presentationQueue;
}

vk::UniqueShaderModule VulkanDevice::CreateShaderModule(const std::vector<char>& shaderCode) const {
	return ::CreateShaderModule(internal->device.get(), shaderCode);
}