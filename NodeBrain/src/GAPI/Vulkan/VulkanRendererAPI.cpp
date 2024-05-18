#include "NBpch.h"
#include "VulkanRendererAPI.h"

#include "Core/App.h"
#include "Renderer/Shader.h"
#include "Renderer/GraphicsPipeline.h"

#include "GAPI/Vulkan/VulkanRenderContext.h"
#include "GAPI/Vulkan/VulkanRenderContext.h"
#include "GAPI/Vulkan/VulkanGraphicsPipeline.h"
#include "GAPI/Vulkan/VulkanComputePipeline.h"

namespace NodeBrain
{
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

		Utils::TransitionImage(m_ActiveCmdBuffer, m_ActiveSwapchainImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);
	}

	void VulkanRendererAPI::EndFrame()
	{
		Utils::TransitionImage(m_ActiveCmdBuffer, m_ActiveSwapchainImage, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

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

	void VulkanRendererAPI::WaitForGPU()
	{
		NB_PROFILE_FN();

		vkDeviceWaitIdle(VulkanRenderContext::Get()->GetVkDevice());
	}

	void VulkanRendererAPI::ClearColor(const glm::vec4& color, std::shared_ptr<Image> image)
	{
		std::shared_ptr<VulkanImage> vulkanImage = image ? std::static_pointer_cast<VulkanImage>(image) : m_Swapchain.GetDrawImage();

		VkClearColorValue clearValue;
		clearValue = { { color.x, color.y, color.z, color.w }};

		VkImageSubresourceRange clearRange = Utils::ImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT);

		vkCmdClearColorImage(m_ActiveCmdBuffer, vulkanImage->GetVkImage(), VK_IMAGE_LAYOUT_GENERAL, &clearValue, 1, &clearRange);
	}

	void VulkanRendererAPI::BeginRenderPass(std::shared_ptr<GraphicsPipeline> pipeline)
	{
		std::shared_ptr<VulkanImage> vulkanImage = pipeline->GetTargetImage() ? std::static_pointer_cast<VulkanImage>(pipeline->GetTargetImage()) : m_Swapchain.GetDrawImage();
		std::shared_ptr<VulkanGraphicsPipeline> vulkanPipeline = std::static_pointer_cast<VulkanGraphicsPipeline>(pipeline);

		vkCmdBindPipeline(m_ActiveCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanPipeline->GetVkPipeline());

		// Update dynamic states
		VkViewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = vulkanImage->GetConfiguration().Width;
		viewport.height = vulkanImage->GetConfiguration().Height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(m_ActiveCmdBuffer, 0, 1, &viewport);

		VkRect2D scissor = {};
		scissor.offset = { 0, 0 };
		scissor.extent = { vulkanImage->GetConfiguration().Width, vulkanImage->GetConfiguration().Height };
		vkCmdSetScissor(m_ActiveCmdBuffer, 0, 1, &scissor);


		// --- Begin rendering ---
		Utils::TransitionImage(m_ActiveCmdBuffer, vulkanImage->GetVkImage(), VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

		VkRenderingAttachmentInfo colorAttachmentInfo = {};
		colorAttachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
		colorAttachmentInfo.imageView = vulkanImage->GetVkImageView(); // Target image
		colorAttachmentInfo.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		colorAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
		colorAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

		VkRenderingInfo renderingInfo = {};
		renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
		renderingInfo.colorAttachmentCount = 1;
		renderingInfo.pColorAttachments = &colorAttachmentInfo;
		renderingInfo.renderArea.extent = { vulkanImage->GetConfiguration().Width, vulkanImage->GetConfiguration().Height };
		renderingInfo.renderArea.offset = { 0, 0 };
		renderingInfo.viewMask = 0;
		renderingInfo.layerCount = 1;

		m_vkCmdBeginRenderingKHR(m_ActiveCmdBuffer, &renderingInfo);
	}

	void VulkanRendererAPI::EndRenderPass(std::shared_ptr<GraphicsPipeline> pipeline)
	{
		std::shared_ptr<VulkanImage> vulkanImage = pipeline->GetTargetImage() ? std::static_pointer_cast<VulkanImage>(pipeline->GetTargetImage()) : m_Swapchain.GetDrawImage();
		
		m_vkCmdEndRenderingKHR(m_ActiveCmdBuffer);

		Utils::TransitionImage(m_ActiveCmdBuffer, vulkanImage->GetVkImage(), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL);



		// temp
		Utils::TransitionImage(m_ActiveCmdBuffer, m_DrawImage, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
		Utils::TransitionImage(m_ActiveCmdBuffer, m_ActiveSwapchainImage, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		Utils::CopyImageToImage(m_ActiveCmdBuffer, m_DrawImage, m_ActiveSwapchainImage, m_Swapchain.GetVkExtent(), m_Swapchain.GetVkExtent());
		Utils::TransitionImage(m_ActiveCmdBuffer, m_DrawImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL);
		Utils::TransitionImage(m_ActiveCmdBuffer, m_ActiveSwapchainImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL);
	}

	void VulkanRendererAPI::Draw(uint32_t vertexCount, uint32_t firstVertex, uint32_t instanceCount, uint32_t instanceIndex)
	{
		vkCmdDraw(m_ActiveCmdBuffer, vertexCount, instanceCount, firstVertex, instanceIndex);
	}

	void VulkanRendererAPI::DrawIndexed(std::shared_ptr<IndexBuffer> indexBuffer, uint32_t indexCount, uint32_t firstIndex, uint32_t instanceCount, uint32_t instanceIndex)
	{
		std::shared_ptr<VulkanIndexBuffer> vulkanIndexBuffer = std::static_pointer_cast<VulkanIndexBuffer>(indexBuffer);

		vkCmdBindIndexBuffer(m_ActiveCmdBuffer, vulkanIndexBuffer->GetVkBuffer(), 0, VK_INDEX_TYPE_UINT32);
		vkCmdDrawIndexed(m_ActiveCmdBuffer, indexCount, instanceCount, firstIndex, 0, instanceIndex);
	}

	void VulkanRendererAPI::BeginComputePass(std::shared_ptr<ComputePipeline> pipeline)
	{
		std::shared_ptr<VulkanComputePipeline> vulkanPipeline = std::static_pointer_cast<VulkanComputePipeline>(pipeline);
		std::shared_ptr<VulkanImage> vulkanImage = pipeline->GetTargetImage() ? std::static_pointer_cast<VulkanImage>(pipeline->GetTargetImage()) : m_Swapchain.GetDrawImage();

		vkCmdBindPipeline(m_ActiveCmdBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, vulkanPipeline->GetVkPipeline());

		Utils::TransitionImage(m_ActiveCmdBuffer, vulkanImage->GetVkImage(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);
	}
	
	void VulkanRendererAPI::EndComputePass(std::shared_ptr<ComputePipeline> pipeline)
	{
		// temp
		Utils::TransitionImage(m_ActiveCmdBuffer, m_DrawImage, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
		Utils::TransitionImage(m_ActiveCmdBuffer, m_ActiveSwapchainImage, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		Utils::CopyImageToImage(m_ActiveCmdBuffer, m_DrawImage, m_ActiveSwapchainImage, m_Swapchain.GetVkExtent(), m_Swapchain.GetVkExtent());
		Utils::TransitionImage(m_ActiveCmdBuffer, m_DrawImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL);
		Utils::TransitionImage(m_ActiveCmdBuffer, m_ActiveSwapchainImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL);
	}

	void VulkanRendererAPI::DispatchCompute(uint32_t groupX, uint32_t groupY, uint32_t groupZ) 
	{
		vkCmdDispatch(m_ActiveCmdBuffer, groupX, groupY, groupZ);
	}
}