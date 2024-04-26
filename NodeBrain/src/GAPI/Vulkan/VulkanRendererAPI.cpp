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
		: m_Swapchain(VulkanRenderContext::Get()->GetSwapchain())
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
		NB_PROFILE_FN();

		vkDeviceWaitIdle(VulkanRenderContext::Get()->GetVkDevice());
	}

	void VulkanRendererAPI::BeginFrame()
	{
		m_ActiveCmdBuffer = m_Swapchain.GetCurrentFrameData().CommandBuffer;
		m_ActiveSwapchainImage = m_Swapchain.GetCurrentImageData().Image;
		m_DrawImage = m_Swapchain.GetDrawImage()->GetVkImage();

		vkResetCommandBuffer(m_ActiveCmdBuffer, 0);

		VkCommandBufferBeginInfo commandBufferBeginInfo = {};
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		commandBufferBeginInfo.flags = 0;
		commandBufferBeginInfo.pInheritanceInfo = nullptr;

		VK_CHECK(vkBeginCommandBuffer(m_ActiveCmdBuffer, &commandBufferBeginInfo));

		TransitionImage(m_ActiveCmdBuffer, m_DrawImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);
		TransitionImage(m_ActiveCmdBuffer, m_ActiveSwapchainImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);
	}

	void VulkanRendererAPI::EndFrame()
	{
		TransitionImage(m_ActiveCmdBuffer, m_ActiveSwapchainImage, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

		VK_CHECK(vkEndCommandBuffer(m_ActiveCmdBuffer));


		const FrameData& frameData = m_Swapchain.GetCurrentFrameData();
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &frameData.ImageAvailableSemaphore;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &frameData.RenderFinishedSemaphore;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_ActiveCmdBuffer;

		VK_CHECK(vkQueueSubmit(VulkanRenderContext::Get()->GetDevice().GetGraphicsQueue(), 1, &submitInfo, frameData.InFlightFence));
	}

	void VulkanRendererAPI::BeginRenderPass()
	{
		// Use swapchain draw image if no target image
		VkImageView drawImageView = m_Swapchain.GetDrawImage()->GetVkImageView();

		TransitionImage(m_ActiveCmdBuffer, m_DrawImage, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

		VkRenderingAttachmentInfo colorAttachmentInfo = {};
		colorAttachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
		colorAttachmentInfo.pNext = nullptr;
		colorAttachmentInfo.imageView = drawImageView; // Target image
		colorAttachmentInfo.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		colorAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
		colorAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

		VkRenderingInfo renderingInfo = {};
		renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
		renderingInfo.pNext = nullptr;
		renderingInfo.colorAttachmentCount = 1;
		renderingInfo.pColorAttachments = &colorAttachmentInfo;
		renderingInfo.renderArea.extent = m_Swapchain.GetVkExtent();
		renderingInfo.renderArea.offset = { 0, 0 };
		renderingInfo.viewMask = 0;
		renderingInfo.layerCount = 1;

		m_vkCmdBeginRenderingKHR(m_ActiveCmdBuffer, &renderingInfo);
	}

	void VulkanRendererAPI::EndRenderPass()
	{
		m_vkCmdEndRenderingKHR(m_ActiveCmdBuffer);

		TransitionImage(m_ActiveCmdBuffer, m_DrawImage, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
		TransitionImage(m_ActiveCmdBuffer, m_ActiveSwapchainImage, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		CopyImageToImage(m_ActiveCmdBuffer, m_DrawImage, m_ActiveSwapchainImage, m_Swapchain.GetVkExtent(), m_Swapchain.GetVkExtent());
		TransitionImage(m_ActiveCmdBuffer, m_DrawImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL);
		TransitionImage(m_ActiveCmdBuffer, m_ActiveSwapchainImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL);
	}

	void VulkanRendererAPI::Draw(uint32_t vertexCount, uint32_t vertexIndex, uint32_t instanceCount, uint32_t instanceIndex)
	{
		vkCmdDraw(m_ActiveCmdBuffer, vertexCount, instanceCount, vertexIndex, instanceIndex);
	}

	void VulkanRendererAPI::BindGraphicsPipeline(std::shared_ptr<GraphicsPipeline> pipeline)
	{
		std::shared_ptr<VulkanGraphicsPipeline> vulkanPipeline = std::static_pointer_cast<VulkanGraphicsPipeline>(pipeline);

		vkCmdBindPipeline(m_ActiveCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanPipeline->GetVkPipeline());

		// Update dynamic states
		VkViewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = m_Swapchain.GetVkExtent().width;
		viewport.height = m_Swapchain.GetVkExtent().height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(m_ActiveCmdBuffer, 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = { m_Swapchain.GetVkExtent().width, m_Swapchain.GetVkExtent().height };
		vkCmdSetScissor(m_ActiveCmdBuffer, 0, 1, &scissor);
	}

	void VulkanRendererAPI::BindComputePipeline(std::shared_ptr<ComputePipeline> pipeline)
	{
		std::shared_ptr<VulkanComputePipeline> vulkanPipeline = std::static_pointer_cast<VulkanComputePipeline>(pipeline);
		std::shared_ptr<VulkanShader> vulkanShader = std::static_pointer_cast<VulkanShader>(vulkanPipeline->GetComputeShader());
		VkPipeline vkPipeline = vulkanPipeline->GetVkPipeline();
		VkPipelineLayout vkPipelineLayout = vulkanPipeline->GetVkPipelineLayout();
		VkDescriptorSet descriptorSets = vulkanShader->GetVkDescriptorSet();

		vkCmdBindPipeline(m_ActiveCmdBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, vkPipeline);
		vkCmdBindDescriptorSets(m_ActiveCmdBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, vkPipelineLayout, 0, 1, &descriptorSets, 0, nullptr);
	}

	void VulkanRendererAPI::BeginComputePass()
	{

	}
	
	void VulkanRendererAPI::EndComputePass()
	{

	}

	void VulkanRendererAPI::DispatchCompute(uint32_t groupX, uint32_t groupY, uint32_t groupZ) 
	{
		vkCmdDispatch(m_ActiveCmdBuffer, groupX, groupY, groupZ);
	}

	void VulkanRendererAPI::ClearColor(const glm::vec4& color)
	{
		VkClearColorValue clearValue;
		clearValue = { { color.x, color.y, color.z, color.w }};

		VkImageSubresourceRange clearRange = ImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT);

		vkCmdClearColorImage(m_ActiveCmdBuffer, m_DrawImage, VK_IMAGE_LAYOUT_GENERAL, &clearValue, 1, &clearRange);
	}

	void VulkanRendererAPI::TempUpdateImage(std::shared_ptr<Shader> shader)
	{
		std::shared_ptr<VulkanShader> vulkanShader = std::dynamic_pointer_cast<VulkanShader>(shader);

		//Temp set image
		VkDescriptorImageInfo imgInfo{};
		imgInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
		imgInfo.imageView = m_Swapchain.GetDrawImage()->GetVkImageView();

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
}