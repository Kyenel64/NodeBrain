#include "NBpch.h"
#include "VulkanRendererAPI.h"

#include "Core/App.h"
#include "Renderer/Shader.h"
#include "Renderer/RenderPass.h"
#include "Renderer/GraphicsPipeline.h"

#include "GAPI/Vulkan/VulkanRenderContext.h"
#include "GAPI/Vulkan/VulkanFramebuffer.h"
#include "GAPI/Vulkan/VulkanSwapchain.h"
#include "GAPI/Vulkan/VulkanRenderContext.h"
#include "GAPI/Vulkan/VulkanGraphicsPipeline.h"

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
		#ifdef NB_VULKAN_VERSION_1_3
			VkImageMemoryBarrier2 imageBarrier = {};
			imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
			imageBarrier.pNext = nullptr;

			imageBarrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
			imageBarrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
			imageBarrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
			imageBarrier.dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT;

			imageBarrier.oldLayout = currentLayout;
			imageBarrier.newLayout = newLayout;

			VkImageAspectFlags aspectFlags = (newLayout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
			imageBarrier.subresourceRange = ImageSubresourceRange(aspectFlags);
			imageBarrier.image = image;

			VkDependencyInfo depInfo = {};
			depInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
			depInfo.pNext = nullptr;
			depInfo.imageMemoryBarrierCount = 1;
			depInfo.pImageMemoryBarriers = &imageBarrier;

			vkCmdPipelineBarrier(commandBuffer, &depInfo);

		#else
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
		#endif
	}

	static void CopyImageToImage(VkCommandBuffer commandBuffer, VkImage srcImage, VkImage dstImage, VkExtent2D srcExtent, VkExtent2D dstExtent)
	{
		#ifdef NB_VULKAN_VERSION_1_3
			VkImageBlit2 blitRegion = {};
			blitRegion.sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2;

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

			VkBlitImageInfo2 blitImageInfo = {};
			blitImageInfo.sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2;
			blitImageInfo.dstImage = dstImage;
			blitImageInfo.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			blitImageInfo.srcImage = srcImage;
			blitImageInfo.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			blitImageInfo.filter = VK_FILTER_LINEAR;
			blitImageInfo.regionCount = 1;
			blitImageInfo.pRegions = &blitRegion;

			vkCmdBlitImage2(commandBuffer, &blitImageInfo);

		#else
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

			vkCmdBlitImage(commandBuffer, 
			srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, 
			dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 
			1, &blitRegion, VK_FILTER_LINEAR);
		#endif
	}

	VulkanRendererAPI::VulkanRendererAPI()
	{
		NB_PROFILE_FN();

		// temp
		ImageConfiguration config = {};
		config.Width = VulkanRenderContext::Get()->GetSwapchain().GetVkExtent().width;
		config.Height = VulkanRenderContext::Get()->GetSwapchain().GetVkExtent().height;
		config.ImageFormat = ImageFormat::RGBA16;
		TempImage = std::make_shared<VulkanImage>(config);
	}

	VulkanRendererAPI::~VulkanRendererAPI()
	{
		NB_PROFILE_FN();
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

		VK_CHECK(vkQueueSubmit(VulkanRenderContext::Get()->GetDevice()->GetGraphicsQueue(), 1, &submitInfo, inFlightFence));
	}

	void VulkanRendererAPI::BeginRenderPass(std::shared_ptr<RenderPass> renderPass)
	{
		VkCommandBuffer cmdBuffer = VulkanRenderContext::Get()->GetSwapchain().GetCurrentFrameData().CommandBuffer;

		VkRenderPass rp = VulkanRenderContext::Get()->GetSwapchain().GetVkRenderPass();
		VkFramebuffer framebuffer = VulkanRenderContext::Get()->GetSwapchain().GetCurrentVkFramebuffer();
			
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

	void VulkanRendererAPI::WaitForGPU()
	{
		vkDeviceWaitIdle(VulkanRenderContext::Get()->GetDevice()->GetVkDevice());
	}

	void VulkanRendererAPI::BeginDynamicPass()
	{
		VkCommandBuffer cmdBuffer = VulkanRenderContext::Get()->GetSwapchain().GetCurrentFrameData().CommandBuffer;
		VkImage swapchainImage = VulkanRenderContext::Get()->GetSwapchain().GetCurrentVkImage();
		VkImage tempImage = TempImage->GetVkImage();

		// Convert image to writable format. Not optimal
		TransitionImage(cmdBuffer, tempImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);
		TransitionImage(cmdBuffer, swapchainImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);
	}
	void VulkanRendererAPI::EndDynamicPass()
	{
		VkCommandBuffer cmdBuffer = VulkanRenderContext::Get()->GetSwapchain().GetCurrentFrameData().CommandBuffer;
		VkImage swapchainImage = VulkanRenderContext::Get()->GetSwapchain().GetCurrentVkImage();
		VkImage tempImage = TempImage->GetVkImage();

		// Convert image to transfer format
		TransitionImage(cmdBuffer, tempImage, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
		TransitionImage(cmdBuffer, swapchainImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

		// Transfer draw image to swapchain image
		CopyImageToImage(cmdBuffer, tempImage, swapchainImage, VulkanRenderContext::Get()->GetSwapchain().GetVkExtent(), VulkanRenderContext::Get()->GetSwapchain().GetVkExtent());

		// Convert newly transferred swapchain image to presentable format
		TransitionImage(cmdBuffer, swapchainImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
	}

	void VulkanRendererAPI::DrawDynamicTest()
	{
		VkCommandBuffer cmdBuffer = VulkanRenderContext::Get()->GetSwapchain().GetCurrentFrameData().CommandBuffer;
		VkImage image = TempImage->GetVkImage();

		VkClearColorValue clearValue;
		clearValue = { { 0.3f, 0.3f, 0.8f, 1.0f }};

		VkImageSubresourceRange clearRange = ImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT);

		vkCmdClearColorImage(cmdBuffer, image, VK_IMAGE_LAYOUT_GENERAL, &clearValue, 1, &clearRange);
	}
}