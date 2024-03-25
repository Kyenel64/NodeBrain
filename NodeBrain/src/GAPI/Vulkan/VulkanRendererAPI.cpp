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
		VK_CHECK(vkBeginCommandBuffer(cmdBuffer, &commandBufferBeginInfo));
	}

	void VulkanRendererAPI::EndFrame()
	{
		VkCommandBuffer cmdBuffer = VulkanRenderContext::GetInstance()->GetSwapchain().GetCurrentFrameData().CommandBuffer;

		// End command buffer
		VK_CHECK(vkEndCommandBuffer(cmdBuffer));


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

		VK_CHECK(vkQueueSubmit(VulkanRenderContext::GetInstance()->GetDevice()->GetGraphicsQueue(), 1, &submitInfo, inFlightFence));
	}

	void VulkanRendererAPI::BeginRenderPass(std::shared_ptr<RenderPass> renderPass)
	{
		VkCommandBuffer cmdBuffer = VulkanRenderContext::GetInstance()->GetSwapchain().GetCurrentFrameData().CommandBuffer;

		VkRenderPass rp = VulkanRenderContext::GetInstance()->GetSwapchain().GetVkRenderPass();
		VkFramebuffer framebuffer = VulkanRenderContext::GetInstance()->GetSwapchain().GetCurrentVkFramebuffer();
			
		VkRenderPassBeginInfo renderPassBeginInfo = {};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.renderPass = rp;
		renderPassBeginInfo.framebuffer = framebuffer;
		renderPassBeginInfo.renderArea.offset = { 0, 0 };
		renderPassBeginInfo.renderArea.extent = { VulkanRenderContext::GetInstance()->GetSwapchain().GetVkExtent().width, VulkanRenderContext::GetInstance()->GetSwapchain().GetVkExtent().height };
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

		// Bind pipeline
		vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, std::dynamic_pointer_cast<VulkanGraphicsPipeline>(pipeline)->GetVkPipeline());


		// Update dynamic states
		VkViewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = VulkanRenderContext::GetInstance()->GetSwapchain().GetVkExtent().width;
		viewport.height = VulkanRenderContext::GetInstance()->GetSwapchain().GetVkExtent().height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(cmdBuffer, 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = { VulkanRenderContext::GetInstance()->GetSwapchain().GetVkExtent().width, VulkanRenderContext::GetInstance()->GetSwapchain().GetVkExtent().height };
		vkCmdSetScissor(cmdBuffer, 0, 1, &scissor);


		// Draw
		vkCmdDraw(cmdBuffer, 3, 1, 0, 0);
	}

	void VulkanRendererAPI::WaitForGPU()
	{
		vkDeviceWaitIdle(VulkanRenderContext::GetInstance()->GetDevice()->GetVkDevice());
	}
}