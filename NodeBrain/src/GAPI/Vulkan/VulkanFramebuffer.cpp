#include "NBpch.h"
#include "VulkanFramebuffer.h"

#include "GAPI/Vulkan/VulkanRenderContext.h"

namespace NodeBrain
{
	VulkanFramebuffer::VulkanFramebuffer(std::shared_ptr<VulkanRenderPass> renderPass, std::shared_ptr<VulkanImage> image)
		: m_RenderPass(renderPass), m_Image(image)
	{
		m_Device = VulkanRenderContext::GetInstance()->GetDevice();

		Init();
	}

	VulkanFramebuffer::~VulkanFramebuffer()
	{
		NB_PROFILE_FN();

		if (m_VkFramebuffer)
		{
			vkDestroyFramebuffer(m_Device->GetVkDevice(), m_VkFramebuffer, nullptr);
			m_VkFramebuffer = VK_NULL_HANDLE;
		}
	}

	void VulkanFramebuffer::Init()
	{
		NB_PROFILE_FN();

		// If using swapchain
		VkImageView attachments[] = { m_Image->GetVkImageView() };

		VkFramebufferCreateInfo framebufferCreateInfo = {};
		framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferCreateInfo.renderPass = m_RenderPass->GetVkRenderPass();
		framebufferCreateInfo.attachmentCount = 1;
		framebufferCreateInfo.pAttachments = attachments;
		framebufferCreateInfo.width = VulkanRenderContext::GetInstance()->GetSwapchain()->GetExtentWidth();
		framebufferCreateInfo.height = VulkanRenderContext::GetInstance()->GetSwapchain()->GetExtentHeight();
		framebufferCreateInfo.layers = 1;

		VkResult result = vkCreateFramebuffer(m_Device->GetVkDevice(), &framebufferCreateInfo, nullptr, &m_VkFramebuffer);
		NB_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan frame buffer");
		
	}
}