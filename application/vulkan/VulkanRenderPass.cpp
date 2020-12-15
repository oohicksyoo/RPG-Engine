//
// Created by Alex on 12/14/2020.
//

#include "VulkanRenderPass.hpp"

using RPG::VulkanRenderPass;

namespace {
	vk::UniqueRenderPass CreateRenderPass(const RPG::VulkanPhysicalDevice& physicalDevice,
										  const RPG::VulkanDevice& device,
										  const RPG::VulkanSwapchain& swapchain) {
		vk::Format colorFormat{ swapchain.GetColorFormat() };
		vk::SampleCountFlagBits multiSamplingLevel{ physicalDevice.GetMultiSamplingLevel() };
		vk::Format depthFormat{ physicalDevice.GetDepthFormat() };

		// Define the 'color' attachment and attachment reference.
		vk::AttachmentDescription colorAttachment{
				vk::AttachmentDescriptionFlags(),          // Flags
				colorFormat,                               // Format
				multiSamplingLevel,                        // Samples
				vk::AttachmentLoadOp::eClear,              // Load operation
				vk::AttachmentStoreOp::eStore,             // Store operation
				vk::AttachmentLoadOp::eDontCare,           // Stencil load operation
				vk::AttachmentStoreOp::eDontCare,          // Stencil store operation
				vk::ImageLayout::eUndefined,               // Initial layout
				vk::ImageLayout::eColorAttachmentOptimal }; // Final layout

		vk::AttachmentReference colorAttachmentReference{
				0,                                         // Attachment index
				vk::ImageLayout::eColorAttachmentOptimal }; // Layout

		// Define the 'depth testing' attachment and reference.
		vk::AttachmentDescription depthTestingAttachment{
				vk::AttachmentDescriptionFlags(),                 // Flags
				depthFormat,                                      // Format
				multiSamplingLevel,                               // Samples
				vk::AttachmentLoadOp::eClear,                     // Load operation
				vk::AttachmentStoreOp::eDontCare,                 // Store operation
				vk::AttachmentLoadOp::eDontCare,                  // Stencil load operation
				vk::AttachmentStoreOp::eDontCare,                 // Stencil store operation
				vk::ImageLayout::eUndefined,                      // Initial layout
				vk::ImageLayout::eDepthStencilAttachmentOptimal }; // Final layout

		vk::AttachmentReference depthTestingAttachmentReference{
				1,                                                // Attachment index
				vk::ImageLayout::eDepthStencilAttachmentOptimal }; // Layout

		// Define the 'multi sampling' attachment and reference.
		vk::AttachmentDescription multiSamplingAttachment{
				vk::AttachmentDescriptionFlags(), // Flags
				colorFormat,                      // Format
				vk::SampleCountFlagBits::e1,      // Samples
				vk::AttachmentLoadOp::eDontCare,  // Load operation
				vk::AttachmentStoreOp::eStore,    // Store operation
				vk::AttachmentLoadOp::eDontCare,  // Stencil load operation
				vk::AttachmentStoreOp::eDontCare, // Stencil store operation
				vk::ImageLayout::eUndefined,      // Initial layout
				vk::ImageLayout::ePresentSrcKHR }; // Final layout

		vk::AttachmentReference multiSamplingAttachmentReference{
				2,                                         // Attachment index
				vk::ImageLayout::eColorAttachmentOptimal }; // Layout

		// Prepare an array of all the attachments that will be used. Note
		// that the 'attachment index' values in the attachment reference
		// objects above correlate to the position of the attachment
		// descriptions in the following array.
		std::array<vk::AttachmentDescription, 3> attachments{
				colorAttachment,
				depthTestingAttachment,
				multiSamplingAttachment };

		// Define the 'subpass' that the render pass will execute.
		vk::SubpassDescription subpass{
				vk::SubpassDescriptionFlags(),     // Flags
				vk::PipelineBindPoint::eGraphics,  // Pipeline bind point
				0,                                 // Input attachment count
				nullptr,                           // Input attachments
				1,                                 // Color attachments count
				&colorAttachmentReference,         // Color attachments
				&multiSamplingAttachmentReference, // Resolve attachments
				&depthTestingAttachmentReference,  // Depth stencil attachments
				0,                                 // Preserve attachments count
				nullptr };                          // Preserve attachments

		// Define the subpass 'dependency' configuration.
		vk::SubpassDependency subpassDependency{
				0,                                                                                    // Source subpass index
				0,                                                                                    // Destination subpass index
				vk::PipelineStageFlagBits::eColorAttachmentOutput,                                    // Source access mask
				vk::PipelineStageFlagBits::eColorAttachmentOutput,                                    // Destination access mask
				vk::AccessFlags(),                                                                    // Source access flags
				vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite, // Destination access flags
				vk::DependencyFlags() };                                                               // Dependency flags

		// Collate the attachments, subpass and subpass dependencies into the
		// configuration object needed to construct a render pass.
		vk::RenderPassCreateInfo renderPassCreateInfo{
				vk::RenderPassCreateFlags(),               // Flags
				static_cast<uint32_t>(attachments.size()), // Attachment count
				attachments.data(),                        // Attachments
				1,                                         // Subpass count
				& subpass,                                  // Subpasses
				1,                                         // Dependency count
				& subpassDependency };                       // Dependencies

		// Ask the logical device to provision a render pass.
		return device.GetDevice().createRenderPassUnique(renderPassCreateInfo);
	}
}

struct VulkanRenderPass::Internal {
	const vk::UniqueRenderPass renderPass;

	Internal(const RPG::VulkanPhysicalDevice& physicalDevice,
			 const RPG::VulkanDevice& device,
			 const RPG::VulkanSwapchain& swapchain)
			: renderPass(::CreateRenderPass(physicalDevice, device, swapchain)) {
	}
};

VulkanRenderPass::VulkanRenderPass(const RPG::VulkanPhysicalDevice& physicalDevice,
								   const RPG::VulkanDevice& device,
								   const RPG::VulkanSwapchain& swapchain)
		: internal(RPG::MakeInternalPointer<Internal>(physicalDevice, device, swapchain)) {
}

const vk::RenderPass& VulkanRenderPass::GetRenderPass() const {
	return internal->renderPass.get();
}