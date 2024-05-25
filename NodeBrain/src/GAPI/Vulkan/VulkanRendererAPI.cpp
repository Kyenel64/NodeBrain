#include "NBpch.h"
#include "VulkanRendererAPI.h"

#include "GAPI/Vulkan/VulkanRenderContext.h"
#include "GAPI/Vulkan/VulkanGraphicsPipeline.h"
#include "GAPI/Vulkan/VulkanComputePipeline.h"
#include "GAPI/Vulkan/VulkanImage.h"
#include "GAPI/Vulkan/VulkanIndexBuffer.h"

namespace NodeBrain
{
	VulkanRendererAPI::VulkanRendererAPI(VulkanRenderContext* renderContext)
		: m_Context(renderContext), m_Swapchain(renderContext->GetSwapchain()) 
	{
		NB_PROFILE_FN();

		m_vkCmdBeginRenderingKHR = (PFN_vkCmdBeginRenderingKHR)vkGetInstanceProcAddr(m_Context->GetVkInstance(), "vkCmdBeginRenderingKHR");
		m_vkCmdEndRenderingKHR = (PFN_vkCmdEndRenderingKHR)vkGetInstanceProcAddr(m_Context->GetVkInstance(), "vkCmdEndRenderingKHR");
	}

	VulkanRendererAPI::~VulkanRendererAPI()
	{
		NB_PROFILE_FN();

		m_Context->WaitForGPU();
	}

	void VulkanRendererAPI::BeginFrame()
	{
		NB_PROFILE_FN();

		m_ActiveCmdBuffer = m_Swapchain.GetCurrentFrameData().CommandBuffer;
		m_ActiveSwapchainImage = m_Swapchain.GetCurrentImageData().Image;
		m_DrawImage = m_Swapchain.GetCurrentFrameData().DrawImage;

		vkResetCommandBuffer(m_ActiveCmdBuffer, 0);

		VkCommandBufferBeginInfo commandBufferBeginInfo = {};
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		commandBufferBeginInfo.flags = 0;
		commandBufferBeginInfo.pInheritanceInfo = nullptr;
		VK_CHECK(vkBeginCommandBuffer(m_ActiveCmdBuffer, &commandBufferBeginInfo));

		Utils::TransitionImage(m_ActiveCmdBuffer, m_ActiveSwapchainImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);
		Utils::TransitionImage(m_ActiveCmdBuffer, m_DrawImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);
	}

	void VulkanRendererAPI::EndFrame()
	{
		NB_PROFILE_FN();

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
		VK_CHECK(vkQueueSubmit(m_Context->GetDevice().GetGraphicsQueue(), 1, &submitInfo, frameData.InFlightFence));
	}

	void VulkanRendererAPI::ClearColor(const glm::vec4& color, std::shared_ptr<Image> image)
	{
		NB_PROFILE_FN();

		VkImage vkImage = image ? dynamic_pointer_cast<VulkanImage>(image)->GetVkImage() : m_Swapchain.GetCurrentFrameData().DrawImage;

		VkClearColorValue clearValue = { { color.x, color.y, color.z, color.w }};
		VkImageSubresourceRange clearRange = Utils::ImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT);
		vkCmdClearColorImage(m_ActiveCmdBuffer, vkImage, VK_IMAGE_LAYOUT_GENERAL, &clearValue, 1, &clearRange);
	}

	void VulkanRendererAPI::BeginRenderPass(std::shared_ptr<GraphicsPipeline> pipeline)
	{
		NB_PROFILE_FN();

		std::shared_ptr<VulkanGraphicsPipeline> vulkanPipeline = dynamic_pointer_cast<VulkanGraphicsPipeline>(pipeline);

		VkImage vkImage = VK_NULL_HANDLE;
		VkImageView vkImageView = VK_NULL_HANDLE;
		VkExtent2D extent = {};
		if (pipeline->GetTargetImage())
		{
			std::shared_ptr<VulkanImage> vulkanImage = dynamic_pointer_cast<VulkanImage>(pipeline->GetTargetImage());
			vkImage = vulkanImage->GetVkImage();
			vkImageView = vulkanImage->GetVkImageView();
			extent = { vulkanImage->GetConfiguration().Width, vulkanImage->GetConfiguration().Height };
		}
		else
		{
			vkImage = m_Swapchain.GetCurrentFrameData().DrawImage;
			vkImageView = m_Swapchain.GetCurrentFrameData().DrawImageView;
			extent = m_Swapchain.GetVkExtent();
		}

		vkCmdBindPipeline(m_ActiveCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanPipeline->GetVkPipeline());

		// Update dynamic states
		VkViewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = extent.width;
		viewport.height = extent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(m_ActiveCmdBuffer, 0, 1, &viewport);

		VkRect2D scissor = {};
		scissor.offset = { 0, 0 };
		scissor.extent = extent;
		vkCmdSetScissor(m_ActiveCmdBuffer, 0, 1, &scissor);


		// --- Begin rendering ---
		Utils::TransitionImage(m_ActiveCmdBuffer, vkImage, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

		VkRenderingAttachmentInfo colorAttachmentInfo = {};
		colorAttachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
		colorAttachmentInfo.imageView = vkImageView;
		colorAttachmentInfo.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		colorAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
		colorAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

		VkRenderingInfo renderingInfo = {};
		renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
		renderingInfo.colorAttachmentCount = 1;
		renderingInfo.pColorAttachments = &colorAttachmentInfo;
		renderingInfo.renderArea.extent = extent;
		renderingInfo.renderArea.offset = { 0, 0 };
		renderingInfo.viewMask = 0;
		renderingInfo.layerCount = 1;

		m_vkCmdBeginRenderingKHR(m_ActiveCmdBuffer, &renderingInfo);
	}

	void VulkanRendererAPI::EndRenderPass(std::shared_ptr<GraphicsPipeline> pipeline)
	{
		NB_PROFILE_FN();

		VkImage vkImage = pipeline->GetTargetImage() ? dynamic_pointer_cast<VulkanImage>(pipeline->GetTargetImage())->GetVkImage() : m_Swapchain.GetCurrentFrameData().DrawImage;
		
		m_vkCmdEndRenderingKHR(m_ActiveCmdBuffer);

		Utils::TransitionImage(m_ActiveCmdBuffer, vkImage, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL);



		// temp
		Utils::TransitionImage(m_ActiveCmdBuffer, m_DrawImage, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
		Utils::TransitionImage(m_ActiveCmdBuffer, m_ActiveSwapchainImage, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		Utils::CopyImageToImage(m_ActiveCmdBuffer, m_DrawImage, m_ActiveSwapchainImage, m_Swapchain.GetVkExtent(), m_Swapchain.GetVkExtent());
		Utils::TransitionImage(m_ActiveCmdBuffer, m_DrawImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL);
		Utils::TransitionImage(m_ActiveCmdBuffer, m_ActiveSwapchainImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL);
	}

	void VulkanRendererAPI::Draw(uint32_t vertexCount, uint32_t firstVertex, uint32_t instanceCount, uint32_t instanceIndex)
	{
		NB_PROFILE_FN();

		vkCmdDraw(m_ActiveCmdBuffer, vertexCount, instanceCount, firstVertex, instanceIndex);
	}

	void VulkanRendererAPI::DrawIndexed(std::shared_ptr<IndexBuffer> indexBuffer, uint32_t indexCount, uint32_t firstIndex, uint32_t instanceCount, uint32_t instanceIndex)
	{
		NB_PROFILE_FN();

		std::shared_ptr<VulkanIndexBuffer> vulkanIndexBuffer = dynamic_pointer_cast<VulkanIndexBuffer>(indexBuffer);

		vkCmdBindIndexBuffer(m_ActiveCmdBuffer, vulkanIndexBuffer->GetVkBuffer(), 0, VK_INDEX_TYPE_UINT32);
		vkCmdDrawIndexed(m_ActiveCmdBuffer, indexCount, instanceCount, firstIndex, 0, instanceIndex);
	}

	void VulkanRendererAPI::BeginComputePass(std::shared_ptr<ComputePipeline> pipeline)
	{
		NB_PROFILE_FN();

		std::shared_ptr<VulkanComputePipeline> vulkanPipeline = dynamic_pointer_cast<VulkanComputePipeline>(pipeline);
		VkImage vkImage = pipeline->GetTargetImage() ? dynamic_pointer_cast<VulkanImage>(pipeline->GetTargetImage())->GetVkImage() : m_Swapchain.GetCurrentFrameData().DrawImage;

		vkCmdBindPipeline(m_ActiveCmdBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, vulkanPipeline->GetVkPipeline());

		Utils::TransitionImage(m_ActiveCmdBuffer, vkImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);
	}
	
	void VulkanRendererAPI::EndComputePass(std::shared_ptr<ComputePipeline> pipeline)
	{
		NB_PROFILE_FN();

		// temp
		Utils::TransitionImage(m_ActiveCmdBuffer, m_DrawImage, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
		Utils::TransitionImage(m_ActiveCmdBuffer, m_ActiveSwapchainImage, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		Utils::CopyImageToImage(m_ActiveCmdBuffer, m_DrawImage, m_ActiveSwapchainImage, m_Swapchain.GetVkExtent(), m_Swapchain.GetVkExtent());
		Utils::TransitionImage(m_ActiveCmdBuffer, m_DrawImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL);
		Utils::TransitionImage(m_ActiveCmdBuffer, m_ActiveSwapchainImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL);
	}

	void VulkanRendererAPI::DispatchCompute(uint32_t groupX, uint32_t groupY, uint32_t groupZ) 
	{
		NB_PROFILE_FN();

		vkCmdDispatch(m_ActiveCmdBuffer, groupX, groupY, groupZ);
	}
}