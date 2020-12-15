//
// Created by Alex on 12/14/2020.
//

#include "VulkanRenderContext.hpp"
#include "VulkanImageView.hpp"
#include "VulkanImage.hpp"
#include "VulkanRenderPass.hpp"
#include <vector>

using RPG::VulkanRenderContext;

namespace {
	RPG::VulkanImage CreateMultiSampleImage(const RPG::VulkanCommandPool& commandPool,
											const RPG::VulkanPhysicalDevice& physicalDevice,
											const RPG::VulkanDevice& device,
											const RPG::VulkanSwapchain& swapchain) {
		const vk::Extent2D& extent{ swapchain.GetExtent() };

		return RPG::VulkanImage(
				commandPool,
				physicalDevice,
				device,
				extent.width,
				extent.height,
				1,
				physicalDevice.GetMultiSamplingLevel(),
				swapchain.GetColorFormat(),
				vk::ImageTiling::eOptimal,
				vk::ImageUsageFlagBits::eTransientAttachment | vk::ImageUsageFlagBits::eColorAttachment,
				vk::MemoryPropertyFlagBits::eDeviceLocal,
				vk::ImageLayout::eUndefined,
				vk::ImageLayout::eColorAttachmentOptimal);
	}

	RPG::VulkanImageView CreateImageView(const RPG::VulkanDevice& device,
										 const RPG::VulkanImage& image,
										 const vk::ImageAspectFlags& aspectFlags) {
		return RPG::VulkanImageView(device.GetDevice(),
									image.GetImage(),
									image.GetFormat(),
									aspectFlags,
									image.GetMipLevels());
	}

	RPG::VulkanImage CreateDepthImage(const RPG::VulkanCommandPool& commandPool,
									  const RPG::VulkanPhysicalDevice& physicalDevice,
									  const RPG::VulkanDevice& device,
									  const RPG::VulkanSwapchain& swapchain) {
		const vk::Extent2D& extent{ swapchain.GetExtent() };

		return RPG::VulkanImage(
				commandPool,
				physicalDevice,
				device,
				extent.width,
				extent.height,
				1,
				physicalDevice.GetMultiSamplingLevel(),
				physicalDevice.GetDepthFormat(),
				vk::ImageTiling::eOptimal,
				vk::ImageUsageFlagBits::eDepthStencilAttachment,
				vk::MemoryPropertyFlagBits::eDeviceLocal,
				vk::ImageLayout::eUndefined,
				vk::ImageLayout::eDepthStencilAttachmentOptimal);
	}

	std::vector<vk::UniqueFramebuffer> CreateFramebuffers(const RPG::VulkanDevice& device,
														  const RPG::VulkanSwapchain& swapchain,
														  const RPG::VulkanRenderPass& renderPass,
														  const RPG::VulkanImageView& multiSampleImageView,
														  const RPG::VulkanImageView& depthImageView) {
		std::vector<vk::UniqueFramebuffer> framebuffers;

		const vk::Extent2D& extent{ swapchain.GetExtent() };

		for (const auto& swapchainImageView : swapchain.GetImageViews()) {
			std::array<vk::ImageView, 3> attachments{
					multiSampleImageView.GetImageView(),
					depthImageView.GetImageView(),
					swapchainImageView.GetImageView()
			};

			vk::FramebufferCreateInfo info{
					vk::FramebufferCreateFlags(),              // Flags
					renderPass.GetRenderPass(),                // Render pass
					static_cast<uint32_t>(attachments.size()), // Attachment count
					attachments.data(),                        // Attachments
					extent.width,                              // Width
					extent.height,                             // Height
					1 };                                        // Layers

			framebuffers.push_back(device.GetDevice().createFramebufferUnique(info));
		}

		return framebuffers;
	}

	vk::Rect2D CreateScissor(const RPG::VulkanSwapchain& swapchain) {
		vk::Offset2D offset{ 0, 0 };

		return vk::Rect2D{
				offset,
				swapchain.GetExtent()
		};
	}

	vk::Viewport CreateViewport(const RPG::VulkanSwapchain& swapchain) {
		const vk::Extent2D extent{ swapchain.GetExtent() };
		const float viewportWidth{ static_cast<float>(extent.width) };
		const float viewportHeight{ static_cast<float>(extent.height) };

		return vk::Viewport{
				0.0f,           // X
				0.0f,           // Y
				viewportWidth,  // Width
				viewportHeight, // Height
				0.0f,           // Min depth
				1.0f };          // Max depth
	}

	std::array<vk::ClearValue, 2> CreateClearValues() {
		//TODO: VULKAN: Background clear colour
		vk::ClearValue color;
		color.color = vk::ClearColorValue(std::array<float, 4>{
				164.0f / 256.0f, // Red
				30.0f / 256.0f,  // Green
				34.0f / 256.0f,  // Blue
				1.0f});          // Alpha

		vk::ClearValue depth;
		depth.depthStencil = vk::ClearDepthStencilValue{ 1.0f, 0 };

		return std::array<vk::ClearValue, 2>{color, depth};
	}

	uint32_t AcquireNextImageIndex(const vk::Device& device,
								   const vk::SwapchainKHR& swapchain,
								   const vk::Fence& fence,
								   const vk::Semaphore& semaphore) {
		#undef max
		static constexpr uint64_t timeOut{ std::numeric_limits<uint64_t>::max() };

		device.waitForFences(
				1,        // Number of fences to wait for
				&fence,   // Fences to wait for
				VK_TRUE,  // Wait for all fences
				timeOut); // Timeout while waiting

		// The fence should now be reset, ready for the next use case.
		device.resetFences(1, &fence);

		vk::ResultValue nextImageIndex{ device.acquireNextImageKHR(
				swapchain, // Swapchain to acquire from
				timeOut,   // Timeout while waiting
				semaphore, // Which semaphore to signal
				nullptr) }; // Which fence to signal

		return nextImageIndex.value;
	}
}

struct VulkanRenderContext::Internal {
	const RPG::VulkanSwapchain swapchain;
	const RPG::VulkanRenderPass renderPass;
	const RPG::VulkanImage multiSampleImage;
	const RPG::VulkanImageView multiSampleImageView;
	const RPG::VulkanImage depthImage;
	const RPG::VulkanImageView depthImageView;
	const std::vector<vk::UniqueFramebuffer> framebuffers;
	const std::vector<vk::UniqueCommandBuffer> commandBuffers;
	const uint32_t maxRenderFrames{ 2 };
	const std::vector<vk::UniqueSemaphore> graphicsSemaphores;
	const std::vector<vk::UniqueSemaphore> presentationSemaphores;
	const std::vector<vk::UniqueFence> graphicsFences;
	const vk::Rect2D scissor;
	const vk::Viewport viewport;
	const std::array<vk::ClearValue, 2> clearValues;

	uint32_t currentFrameIndex{ 0 };
	uint32_t currentSwapchainImageIndex{ 0 };

	Internal(const RPG::SDLWindow& window,
			 const RPG::VulkanPhysicalDevice& physicalDevice,
			 const RPG::VulkanDevice& device,
			 const RPG::VulkanSurface& surface,
			 const RPG::VulkanCommandPool& commandPool,
			 const vk::SwapchainKHR& oldSwapchain)
			: swapchain(RPG::VulkanSwapchain(window, physicalDevice, device, surface, oldSwapchain)),
			  renderPass(RPG::VulkanRenderPass(physicalDevice, device, swapchain)),
			  multiSampleImage(::CreateMultiSampleImage(commandPool, physicalDevice, device, swapchain)),
			  multiSampleImageView(::CreateImageView(device, multiSampleImage, vk::ImageAspectFlagBits::eColor)),
			  depthImage(::CreateDepthImage(commandPool, physicalDevice, device, swapchain)),
			  depthImageView(::CreateImageView(device, depthImage, vk::ImageAspectFlagBits::eDepth)),
			  framebuffers(::CreateFramebuffers(device, swapchain, renderPass, multiSampleImageView, depthImageView)),
			  commandBuffers(commandPool.CreateCommandBuffers(device, swapchain.GetImageCount())),
			  graphicsSemaphores(device.CreateSemaphores(maxRenderFrames)),
			  presentationSemaphores(device.CreateSemaphores(maxRenderFrames)),
			  graphicsFences(device.CreateFences(maxRenderFrames)),
			  scissor(::CreateScissor(swapchain)),
			  viewport(::CreateViewport(swapchain)),
			  clearValues(::CreateClearValues()) {
	}

	const vk::CommandBuffer& GetActiveCommandBuffer() const {
		return commandBuffers[currentSwapchainImageIndex].get();
	}

	bool RenderBegin(const RPG::VulkanDevice& device) {
		// Get the appropriate graphics fence and semaphore for the current render frame.
		const vk::Fence& graphicsFence{ graphicsFences[currentFrameIndex].get() };
		const vk::Semaphore& graphicsSemaphore{ graphicsSemaphores[currentFrameIndex].get() };

		try {
			// Attempt to acquire the next swapchain image index to target.
			currentSwapchainImageIndex = ::AcquireNextImageIndex(device.GetDevice(),
																 swapchain.GetSwapchain(),
																 graphicsFence,
																 graphicsSemaphore);
		} catch (vk::OutOfDateKHRError outOfDateError) {
			// We cannot render with the current swapchain - it needs to be recreated.
			return false;
		}

		// Grab the command buffer to use for the current swapchain image index.
		const vk::CommandBuffer& commandBuffer{ GetActiveCommandBuffer() };

		// Reset the command buffer to a fresh state.
		commandBuffer.reset(vk::CommandBufferResetFlagBits::eReleaseResources);

		// Begin the command buffer.
		vk::CommandBufferBeginInfo commandBufferBeginInfo{ vk::CommandBufferUsageFlagBits::eOneTimeSubmit, nullptr };
		commandBuffer.begin(&commandBufferBeginInfo);

		// Configure the scissor.
		commandBuffer.setScissor(
				0,         // Which scissor to start at
				1,         // How many scissors to apply
				&scissor); // Scissor data

		// Configure the viewport.
		commandBuffer.setViewport(
				0,          // Which viewport to start at
				1,          // How many viewports to apply
				&viewport); // Viewport data

		// Define the render pass attributes to apply.
		vk::RenderPassBeginInfo renderPassBeginInfo{
				renderPass.GetRenderPass(),                     // Render pass to use
				framebuffers[currentSwapchainImageIndex].get(), // Current frame buffer
				scissor,                                        // Render area
				2,                                              // Clear value count
				clearValues.data() };                            // Clear values

		// Record the begin render pass command.
		commandBuffer.beginRenderPass(&renderPassBeginInfo, vk::SubpassContents::eInline);

		return true;
	}

	bool RenderEnd(const RPG::VulkanDevice& device) {
		// Grab the command buffer to use for the current swapchain image index.
		const vk::CommandBuffer& commandBuffer{ GetActiveCommandBuffer() };

		// Request the command buffer to end its recording phase.
		commandBuffer.endRenderPass();
		commandBuffer.end();

		// Get the appropriate graphics fence and semaphores for the current render frame.
		const vk::Fence& graphicsFence{ graphicsFences[currentFrameIndex].get() };
		const vk::Semaphore& graphicsSemaphore{ graphicsSemaphores[currentFrameIndex].get() };
		const vk::Semaphore& presentationSemaphore{ presentationSemaphores[currentFrameIndex].get() };
		const vk::PipelineStageFlags pipelineStageFlags{ vk::PipelineStageFlagBits::eColorAttachmentOutput };

		// Build a submission object for the graphics queue to process.
		vk::SubmitInfo submitInfo{
				1,                       // Wait semaphore count
				&graphicsSemaphore,      // Wait semaphores
				&pipelineStageFlags,     // Pipeline stage flags
				1,                       // Command buffer count
				&commandBuffer,          // Command buffer
				1,                       // Signal semaphore count
				&presentationSemaphore }; // Signal semaphores

		// Submit our command buffer and configuration to the graphics queue.
		device.GetGraphicsQueue().submit(1, &submitInfo, graphicsFence);

		// Construct an info object to describe what to present to the screen.
		vk::PresentInfoKHR presentationInfo{
				1,                           // Semaphore count
				&presentationSemaphore,      // Wait semaphore
				1,                           // Swapchain count
				&swapchain.GetSwapchain(),   // Swapchain
				&currentSwapchainImageIndex, // Image indices
				nullptr };                    // Results

		try {
			// Attempt to submit our graphics output to the presentation queue for display.
			// If we receive an out of date error, or the result comes back as sub optimal
			// we will return false as it indicates our swapchain should be recreated.
			if (device.GetPresentationQueue().presentKHR(presentationInfo) == vk::Result::eSuboptimalKHR) {
				return false;
			}
		} catch (vk::OutOfDateKHRError outOfDateError) {
			return false;
		}

		// We now wait for the presentation to have been completed before continuing.
		device.GetPresentationQueue().waitIdle();

		// Increment our current frame index, wrapping it when it hits our maximum.
		currentFrameIndex = (currentFrameIndex + 1) % maxRenderFrames;

		return true;
	}
};

VulkanRenderContext::VulkanRenderContext(const RPG::SDLWindow& window,
										 const RPG::VulkanPhysicalDevice& physicalDevice,
										 const RPG::VulkanDevice& device,
										 const RPG::VulkanSurface& surface,
										 const RPG::VulkanCommandPool& commandPool,
										 const vk::SwapchainKHR& oldSwapchain)
		: internal(RPG::MakeInternalPointer<Internal>(window, physicalDevice, device, surface, commandPool, oldSwapchain)) {
}

bool VulkanRenderContext::RenderBegin(const RPG::VulkanDevice& device) {
	return internal->RenderBegin(device);
}

bool VulkanRenderContext::RenderEnd(const RPG::VulkanDevice& device) {
	return internal->RenderEnd(device);
}

RPG::VulkanRenderContext VulkanRenderContext::Recreate(const RPG::SDLWindow& window,
													   const RPG::VulkanPhysicalDevice& physicalDevice,
													   const RPG::VulkanDevice& device,
													   const RPG::VulkanSurface& surface,
													   const RPG::VulkanCommandPool& commandPool) {
	return RPG::VulkanRenderContext(window,
									physicalDevice,
									device,
									surface,
									commandPool,
									internal->swapchain.GetSwapchain());
}

const vk::Viewport& VulkanRenderContext::GetViewport() const {
	return internal->viewport;
}

const vk::Rect2D& VulkanRenderContext::GetScissor() const {
	return internal->scissor;
}

const vk::RenderPass& VulkanRenderContext::GetRenderPass() const {
	return internal->renderPass.GetRenderPass();
}

const vk::CommandBuffer& VulkanRenderContext::GetActiveCommandBuffer() const {
	return internal->GetActiveCommandBuffer();
}