#include "NBpch.h"
#include "VulkanCommandBuffer.h"

#include "GAPI/Vulkan/VulkanRenderContext.h"

namespace NodeBrain
{
	VulkanCommandBuffer::VulkanCommandBuffer(std::shared_ptr<VulkanCommandPool> commandPool)
		: m_CommandPool(commandPool)
	{
		m_Device = VulkanRenderContext::GetInstance()->GetDevice();

		Init();
	}

	void VulkanCommandBuffer::Init()
	{
		NB_PROFILE_FN();

		VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
		commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferAllocateInfo.commandPool = m_CommandPool->GetVkCommandPool();
		commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		commandBufferAllocateInfo.commandBufferCount = 1;

		VkResult result = vkAllocateCommandBuffers(m_Device->GetVkDevice(), &commandBufferAllocateInfo, &m_VkCommandBuffer);
		NB_ASSERT(result == VK_SUCCESS, result);
	}

	VulkanCommandBuffer::~VulkanCommandBuffer()
	{
		
	}

	void VulkanCommandBuffer::Begin()
	{
		VkCommandBufferBeginInfo commandBufferBeginInfo = {};
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		commandBufferBeginInfo.flags = 0;
		commandBufferBeginInfo.pInheritanceInfo = nullptr;

		VkResult result = vkBeginCommandBuffer(m_VkCommandBuffer, &commandBufferBeginInfo);
		NB_ASSERT(result == VK_SUCCESS, result);
	}

	void VulkanCommandBuffer::End()
	{
		VkResult result = vkEndCommandBuffer(m_VkCommandBuffer);
		NB_ASSERT(result == VK_SUCCESS, result);
	}

	void VulkanCommandBuffer::StartRenderPass(std::shared_ptr<VulkanRenderPass> renderPass, std::shared_ptr<VulkanFramebuffer> framebuffer)
	{
		VkRenderPassBeginInfo renderPassBeginInfo = {};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.renderPass = renderPass->GetVkRenderPass();
		renderPassBeginInfo.framebuffer = framebuffer->GetVkFramebuffer();
		renderPassBeginInfo.renderArea.offset = { 0, 0 };
		renderPassBeginInfo.renderArea.extent = VulkanRenderContext::GetInstance()->GetSwapchain()->GetVkExtent();
		VkClearValue clearColor = { 0.3f, 0.3f, 0.3f, 1.0f };
		renderPassBeginInfo.clearValueCount = 1;
		renderPassBeginInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(m_VkCommandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = VulkanRenderContext::GetInstance()->GetSwapchain()->GetExtentWidth();
		viewport.height = VulkanRenderContext::GetInstance()->GetSwapchain()->GetExtentHeight();
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(m_VkCommandBuffer, 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = VulkanRenderContext::GetInstance()->GetSwapchain()->GetVkExtent();
		vkCmdSetScissor(m_VkCommandBuffer, 0, 1, &scissor);
	}

	void VulkanCommandBuffer::EndRenderPass()
	{
		vkCmdEndRenderPass(m_VkCommandBuffer);
	}

	void VulkanCommandBuffer::Draw()
	{
		vkCmdDraw(m_VkCommandBuffer, 3, 1, 0, 0);
	}

	void VulkanCommandBuffer::BindGraphicsPipeline(std::shared_ptr<VulkanGraphicsPipeline> pipeline)
	{
		vkCmdBindPipeline(m_VkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetVkPipeline());
	}
}