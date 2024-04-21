#include "NBpch.h"
#include "VulkanRendererAPI.h"

#include <ImGui/imgui.h> // temp
#include <ImGui/backends/imgui_impl_vulkan.h>

#include "Core/App.h"
#include "Renderer/Shader.h"
#include "Renderer/RenderPass.h"
#include "Renderer/GraphicsPipeline.h"

#include "GAPI/Vulkan/VulkanRenderContext.h"
#include "GAPI/Vulkan/VulkanFramebuffer.h"
#include "GAPI/Vulkan/VulkanRenderContext.h"
#include "GAPI/Vulkan/VulkanGraphicsPipeline.h"
#include "GAPI/Vulkan/VulkanComputePipeline.h"
#include "GAPI/Vulkan/VulkanSwapchain.h"

namespace NodeBrain
{
	static VkImageSubresourceRange ImageSubresourceRange(VkImageAspectFlags aspectMask)
	{
		VkImageSubresourceRange subImage = {};
		subImage.aspectMask = aspectMask;
		subImage.baseMipLevel = 0;
		subImage.levelCount = VK_REMAINING_MIP_LEVELS;
		subImage.baseArrayLayer = 0;
		subImage.layerCount = VK_REMAINING_ARRAY_LAYERS;

		return subImage;
	}

	static void TransitionImage(VkCommandBuffer commandBuffer, VkImage image, VkImageLayout currentLayout, VkImageLayout newLayout)
	{
		VkImageMemoryBarrier imageBarrier = {};
		imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		imageBarrier.pNext = nullptr;

		imageBarrier.srcAccessMask = VK_ACCESS_MEMORY_WRITE_BIT;
		imageBarrier.dstAccessMask = VK_ACCESS_MEMORY_WRITE_BIT | VK_ACCESS_MEMORY_READ_BIT;
		imageBarrier.oldLayout = currentLayout;
		imageBarrier.newLayout = newLayout;

		VkImageAspectFlags aspectFlags = (newLayout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
		imageBarrier.subresourceRange = ImageSubresourceRange(aspectFlags);
		imageBarrier.image = image;

		vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageBarrier);
	}

	static void CopyImageToImage(VkCommandBuffer commandBuffer, VkImage srcImage, VkImage dstImage, VkExtent2D srcExtent, VkExtent2D dstExtent)
	{
		VkImageBlit blitRegion = {};
		blitRegion.srcOffsets[1].x = srcExtent.width;
		blitRegion.srcOffsets[1].y = srcExtent.height;
		blitRegion.srcOffsets[1].z = 1;

		blitRegion.dstOffsets[1].x = dstExtent.width;
		blitRegion.dstOffsets[1].y = dstExtent.height;
		blitRegion.dstOffsets[1].z = 1;

		blitRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blitRegion.srcSubresource.baseArrayLayer = 0;
		blitRegion.srcSubresource.layerCount = 1;
		blitRegion.srcSubresource.mipLevel = 0;

		blitRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blitRegion.dstSubresource.baseArrayLayer = 0;
		blitRegion.dstSubresource.layerCount = 1;
		blitRegion.dstSubresource.mipLevel = 0;

		vkCmdBlitImage(commandBuffer, srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blitRegion, VK_FILTER_LINEAR);
	}

	VulkanRendererAPI::VulkanRendererAPI()
	{
		NB_PROFILE_FN();

		VkInstance vkInstance = VulkanRenderContext::Get()->GetVkInstance();
		m_vkCmdBeginRenderingKHR = (PFN_vkCmdBeginRenderingKHR)vkGetInstanceProcAddr(vkInstance, "vkCmdBeginRenderingKHR");
		m_vkCmdEndRenderingKHR = (PFN_vkCmdEndRenderingKHR)vkGetInstanceProcAddr(vkInstance, "vkCmdEndRenderingKHR");
	}

	VulkanRendererAPI::~VulkanRendererAPI()
	{
		NB_PROFILE_FN();
	}

	void VulkanRendererAPI::WaitForGPU()
	{
		vkDeviceWaitIdle(VulkanRenderContext::Get()->GetVkDevice());
	}

	void VulkanRendererAPI::BeginFrame()
	{
		VkCommandBuffer cmdBuffer = VulkanRenderContext::Get()->GetSwapchain().GetCurrentFrameData().CommandBuffer;

		VkCommandBufferBeginInfo commandBufferBeginInfo = {};
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		commandBufferBeginInfo.flags = 0;
		commandBufferBeginInfo.pInheritanceInfo = nullptr;

		vkResetCommandBuffer(cmdBuffer, 0);
		VK_CHECK(vkBeginCommandBuffer(cmdBuffer, &commandBufferBeginInfo));
	}

	void VulkanRendererAPI::EndFrame()
	{
		VkCommandBuffer cmdBuffer = VulkanRenderContext::Get()->GetSwapchain().GetCurrentFrameData().CommandBuffer;
		VkImage swapchainImage = VulkanRenderContext::Get()->GetSwapchain().GetCurrentImageData().Image;

		TransitionImage(cmdBuffer, swapchainImage, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

		// End command buffer
		VK_CHECK(vkEndCommandBuffer(cmdBuffer));


		// Submit buffer
		VkFence inFlightFence = VulkanRenderContext::Get()->GetSwapchain().GetCurrentFrameData().InFlightFence;
		VkSemaphore imageAvailableSemaphore = VulkanRenderContext::Get()->GetSwapchain().GetCurrentFrameData().ImageAvailableSemaphore;
		VkSemaphore renderFinishedSemaphore = VulkanRenderContext::Get()->GetSwapchain().GetCurrentFrameData().RenderFinishedSemaphore;

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { imageAvailableSemaphore };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &cmdBuffer;

		VkSemaphore signalSemaphores[] = { renderFinishedSemaphore };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		VK_CHECK(vkQueueSubmit(VulkanRenderContext::Get()->GetDevice().GetGraphicsQueue(), 1, &submitInfo, inFlightFence));
	}

	void VulkanRendererAPI::BeginRenderPass(std::shared_ptr<RenderPass> renderPass)
	{
		VkCommandBuffer cmdBuffer = VulkanRenderContext::Get()->GetSwapchain().GetCurrentFrameData().CommandBuffer;

		VkRenderPass rp = VulkanRenderContext::Get()->GetSwapchain().GetVkRenderPass();
		VkFramebuffer framebuffer = VulkanRenderContext::Get()->GetSwapchain().GetCurrentImageData().Framebuffer;
			
		VkRenderPassBeginInfo renderPassBeginInfo = {};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.renderPass = rp;
		renderPassBeginInfo.framebuffer = framebuffer;
		renderPassBeginInfo.renderArea.offset = { 0, 0 };
		renderPassBeginInfo.renderArea.extent = { VulkanRenderContext::Get()->GetSwapchain().GetVkExtent().width, VulkanRenderContext::Get()->GetSwapchain().GetVkExtent().height };
		VkClearValue clearColor = { 0.3f, 0.3f, 0.3f, 1.0f };
		renderPassBeginInfo.clearValueCount = 1;
		renderPassBeginInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(cmdBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
	}

	void VulkanRendererAPI::EndRenderPass()
	{
		VkCommandBuffer cmdBuffer = VulkanRenderContext::Get()->GetSwapchain().GetCurrentFrameData().CommandBuffer;

		vkCmdEndRenderPass(cmdBuffer);
	}

	void VulkanRendererAPI::DrawTestTriangle(std::shared_ptr<GraphicsPipeline> pipeline)
	{
		VkCommandBuffer cmdBuffer = VulkanRenderContext::Get()->GetSwapchain().GetCurrentFrameData().CommandBuffer;

		// Bind pipeline
		vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, std::dynamic_pointer_cast<VulkanGraphicsPipeline>(pipeline)->GetVkPipeline());


		// Update dynamic states
		VkViewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = VulkanRenderContext::Get()->GetSwapchain().GetVkExtent().width;
		viewport.height = VulkanRenderContext::Get()->GetSwapchain().GetVkExtent().height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(cmdBuffer, 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = { VulkanRenderContext::Get()->GetSwapchain().GetVkExtent().width, VulkanRenderContext::Get()->GetSwapchain().GetVkExtent().height };
		vkCmdSetScissor(cmdBuffer, 0, 1, &scissor);


		// Draw
		vkCmdDraw(cmdBuffer, 3, 1, 0, 0);
	}

	void VulkanRendererAPI::BeginComputePass(std::shared_ptr<ComputePipeline> pipeline)
	{
		std::shared_ptr<VulkanComputePipeline> vulkanPipeline = std::static_pointer_cast<VulkanComputePipeline>(pipeline);
		std::shared_ptr<VulkanShader> vulkanShader = std::static_pointer_cast<VulkanShader>(vulkanPipeline->GetComputeShader());
		VulkanSwapchain& swapchain = VulkanRenderContext::Get()->GetSwapchain();
		VkCommandBuffer cmdBuffer = swapchain.GetCurrentFrameData().CommandBuffer;
		VkImage swapchainImage = swapchain.GetCurrentImageData().Image;
		VkImage drawImage = swapchain.GetDrawImage()->GetVkImage();
		VkPipeline vkPipeline = vulkanPipeline->GetVkPipeline();
		VkPipelineLayout vkPipelineLayout = vulkanPipeline->GetVkPipelineLayout();
		VkDescriptorSet descriptorSets = vulkanShader->GetVkDescriptorSet();

		// Convert image to writable format. Not optimal
		TransitionImage(cmdBuffer, drawImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);
		TransitionImage(cmdBuffer, swapchainImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

		vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, vkPipeline);

		vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, vkPipelineLayout, 0, 1, &descriptorSets, 0, nullptr);
	}
	
	void VulkanRendererAPI::EndComputePass()
	{
		VulkanSwapchain& swapchain = VulkanRenderContext::Get()->GetSwapchain();
		VkCommandBuffer cmdBuffer = swapchain.GetCurrentFrameData().CommandBuffer;
		VkImage swapchainImage = swapchain.GetCurrentImageData().Image;
		VkImage drawImage = swapchain.GetDrawImage()->GetVkImage();

		// Convert image to transfer format
		TransitionImage(cmdBuffer, drawImage, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
		TransitionImage(cmdBuffer, swapchainImage, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

		// Transfer draw image to swapchain image
		CopyImageToImage(cmdBuffer, drawImage, swapchainImage, swapchain.GetVkExtent(), swapchain.GetVkExtent());

		// Convert newly copied swapchain image to presentable format
		TransitionImage(cmdBuffer, swapchainImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL);
		TransitionImage(cmdBuffer, drawImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL);
	}

	void VulkanRendererAPI::DispatchCompute(uint32_t groupX, uint32_t groupY, uint32_t groupZ) 
	{
		VkCommandBuffer cmdBuffer = VulkanRenderContext::Get()->GetSwapchain().GetCurrentFrameData().CommandBuffer;

		vkCmdDispatch(cmdBuffer, groupX, groupY, groupZ);
	}

	void VulkanRendererAPI::ClearColor(const glm::vec4& color)
	{
		VulkanSwapchain& swapchain = VulkanRenderContext::Get()->GetSwapchain();
		VkCommandBuffer cmdBuffer = swapchain.GetCurrentFrameData().CommandBuffer;
		VkImage drawImage = swapchain.GetDrawImage()->GetVkImage();

		VkClearColorValue clearValue;
		clearValue = { { color.x, color.y, color.z, color.w }};

		VkImageSubresourceRange clearRange = ImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT);

		vkCmdClearColorImage(cmdBuffer, drawImage, VK_IMAGE_LAYOUT_GENERAL, &clearValue, 1, &clearRange);
	}

	void VulkanRendererAPI::TempUpdateImage(std::shared_ptr<Shader> shader)
	{
		VkImageView drawImageView = VulkanRenderContext::Get()->GetSwapchain().GetDrawImage()->GetVkImageView();
		std::shared_ptr<VulkanShader> vulkanShader = std::dynamic_pointer_cast<VulkanShader>(shader);

		//Temp set image
		VkDescriptorImageInfo imgInfo{};
		imgInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
		imgInfo.imageView = drawImageView;

		VkWriteDescriptorSet drawImageWrite = {};
		drawImageWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		drawImageWrite.pNext = nullptr;

		drawImageWrite.dstBinding = 0;
		drawImageWrite.dstSet = vulkanShader->GetVkDescriptorSet();
		drawImageWrite.descriptorCount = 1;
		drawImageWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		drawImageWrite.pImageInfo = &imgInfo;

		vkUpdateDescriptorSets(VulkanRenderContext::Get()->GetVkDevice(), 1, &drawImageWrite, 0, nullptr);
	}

	void VulkanRendererAPI::DrawGUI()
	{
		VulkanSwapchain& swapchain = VulkanRenderContext::Get()->GetSwapchain();
		VkImage swapchainImage = swapchain.GetCurrentImageData().Image;
		VkImageView swapchainImageView = swapchain.GetCurrentImageData().ImageView;
		VkCommandBuffer cmdBuffer = swapchain.GetCurrentFrameData().CommandBuffer;

		VkRenderingAttachmentInfo colorAttachmentInfo = {};
		colorAttachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
		colorAttachmentInfo.pNext = nullptr;
		colorAttachmentInfo.imageView = swapchainImageView;
		colorAttachmentInfo.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		colorAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
		colorAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

		VkRenderingInfo renderingInfo = {};
		renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
		renderingInfo.pNext = nullptr;
		renderingInfo.colorAttachmentCount = 1;
		renderingInfo.pColorAttachments = &colorAttachmentInfo;
		renderingInfo.renderArea.extent = swapchain.GetVkExtent();
		renderingInfo.renderArea.offset = { 0, 0 };
		renderingInfo.viewMask = 0;
		renderingInfo.layerCount = 1;

		TransitionImage(cmdBuffer, swapchainImage, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

		m_vkCmdBeginRenderingKHR(cmdBuffer, &renderingInfo);

		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmdBuffer);

		m_vkCmdEndRenderingKHR(cmdBuffer);

		TransitionImage(cmdBuffer, swapchainImage, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL);
	}
}