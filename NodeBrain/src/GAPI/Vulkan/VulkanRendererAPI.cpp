#include "NBpch.h"
#include "VulkanRendererAPI.h"

#include "Core/App.h"
#include "Renderer/Shader.h"
#include "Renderer/RenderPass.h"
#include "Renderer/GraphicsPipeline.h"

#include "GAPI/Vulkan/VulkanRenderContext.h"
#include "GAPI/Vulkan/VulkanFramebuffer.h"
#include "GAPI/Vulkan/VulkanSwapChain.h"
#include "GAPI/Vulkan/VulkanRenderContext.h"
#include "GAPI/Vulkan/VulkanGraphicsPipeline.h"

#define NB_LIST_AVAILABLE_VK_EXTENSTIONS

namespace NodeBrain
{
	VulkanRendererAPI::VulkanRendererAPI()
	{
		NB_PROFILE_FN();

		Init();
	}

	VulkanRendererAPI::~VulkanRendererAPI()
	{
		NB_PROFILE_FN();
	}

	void VulkanRendererAPI::Init()
	{
		NB_PROFILE_FN();
	}

	void VulkanRendererAPI::BeginFrame()
	{
		VkCommandBuffer cmdBuffer = VulkanRenderContext::GetInstance()->GetSwapchain().GetCurrentFrameData().CommandBuffer;

		VkCommandBufferBeginInfo commandBufferBeginInfo = {};
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		commandBufferBeginInfo.flags = 0;
		commandBufferBeginInfo.pInheritanceInfo = nullptr;

		vkResetCommandBuffer(cmdBuffer, 0);
		VkResult result = vkBeginCommandBuffer(cmdBuffer, &commandBufferBeginInfo);
		NB_ASSERT(result == VK_SUCCESS, result);
	}

	void VulkanRendererAPI::EndFrame()
	{
		VkCommandBuffer cmdBuffer = VulkanRenderContext::GetInstance()->GetSwapchain().GetCurrentFrameData().CommandBuffer;

		// End command buffer
		VkResult result = vkEndCommandBuffer(cmdBuffer);
		NB_ASSERT(result == VK_SUCCESS, result);


		// Submit buffer
		VkFence inFlightFence = VulkanRenderContext::GetInstance()->GetSwapchain().GetCurrentFrameData().InFlightFence;
		VkSemaphore imageAvailableSemaphore = VulkanRenderContext::GetInstance()->GetSwapchain().GetCurrentFrameData().ImageAvailableSemaphore;
		VkSemaphore renderFinishedSemaphore = VulkanRenderContext::GetInstance()->GetSwapchain().GetCurrentFrameData().RenderFinishedSemaphore;

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

		result = vkQueueSubmit(VulkanRenderContext::GetInstance()->GetDevice()->GetGraphicsQueue(), 1, &submitInfo, inFlightFence);
		NB_ASSERT(result == VK_SUCCESS, result);
	}

	void VulkanRendererAPI::BeginRenderPass(std::shared_ptr<RenderPass> renderPass)
	{
		VkCommandBuffer cmdBuffer = VulkanRenderContext::GetInstance()->GetSwapchain().GetCurrentFrameData().CommandBuffer;

		std::shared_ptr<VulkanRenderPass> rp;
		if (!renderPass)
			rp = VulkanRenderContext::GetInstance()->GetSwapchain().GetRenderPass();
		else
			NB_ASSERT(false, "Not yet implemented");
			
		std::shared_ptr<VulkanFramebuffer> targetFramebuffer = rp->GetTargetFramebuffer();

		VkRenderPassBeginInfo renderPassBeginInfo = {};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.renderPass = rp->GetVkRenderPass();
		renderPassBeginInfo.framebuffer = targetFramebuffer->GetVkFramebuffer();
		renderPassBeginInfo.renderArea.offset = { 0, 0 };
		renderPassBeginInfo.renderArea.extent = { targetFramebuffer->GetConfiguration().Width, targetFramebuffer->GetConfiguration().Height };
		VkClearValue clearColor = { 0.3f, 0.3f, 0.3f, 1.0f };
		renderPassBeginInfo.clearValueCount = 1;
		renderPassBeginInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(cmdBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
	}

	void VulkanRendererAPI::EndRenderPass()
	{
		VkCommandBuffer cmdBuffer = VulkanRenderContext::GetInstance()->GetSwapchain().GetCurrentFrameData().CommandBuffer;

		vkCmdEndRenderPass(cmdBuffer);
	}

	void VulkanRendererAPI::DrawTestTriangle(std::shared_ptr<GraphicsPipeline> pipeline)
	{
		VkCommandBuffer cmdBuffer = VulkanRenderContext::GetInstance()->GetSwapchain().GetCurrentFrameData().CommandBuffer;
		const PipelineConfiguration& config = pipeline->GetConfiguration();
		std::shared_ptr<VulkanRenderPass> renderPass = std::dynamic_pointer_cast<VulkanRenderPass>(pipeline->GetConfiguration().Framebuffer->GetConfiguration().RenderPass);
		std::shared_ptr<VulkanFramebuffer> targetFramebuffer = renderPass->GetTargetFramebuffer();

		// Bind pipeline
		vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, std::dynamic_pointer_cast<VulkanGraphicsPipeline>(pipeline)->GetVkPipeline());


		// Update dynamic states
		VkViewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = targetFramebuffer->GetConfiguration().Width;
		viewport.height = targetFramebuffer->GetConfiguration().Height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(cmdBuffer, 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = { targetFramebuffer->GetConfiguration().Width, targetFramebuffer->GetConfiguration().Height };
		vkCmdSetScissor(cmdBuffer, 0, 1, &scissor);


		// Draw
		vkCmdDraw(cmdBuffer, 3, 1, 0, 0);
	}

	void VulkanRendererAPI::WaitForGPU()
	{
		//VkFence fence = VulkanRenderContext::GetInstance()->GetSwapchain().GetInFlightFence();
		//vkWaitForFences(VulkanRenderContext::GetInstance()->GetDevice()->GetVkDevice(), 1, &fence, VK_TRUE, UINT64_MAX);
		vkDeviceWaitIdle(VulkanRenderContext::GetInstance()->GetDevice()->GetVkDevice());
	}
}