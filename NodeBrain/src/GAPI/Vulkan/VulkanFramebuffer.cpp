#include "NBpch.h"
#include "VulkanFramebuffer.h"

#include "GAPI/Vulkan/VulkanRenderContext.h"

namespace NodeBrain
{
	VulkanFramebuffer::VulkanFramebuffer(const FramebufferConfiguration& configuration)
		: m_Configuration(configuration)
	{
		NB_PROFILE_FN();
		
		// Create a new render pass if no render pass is provided
		if (!m_Configuration.TargetRenderPass)
		{
			m_Configuration.TargetRenderPass = std::make_shared<VulkanRenderPass>();
		}

		// Currently used for creating framebuffers from swapchain images
		if (!m_Configuration.TargetImage)
		{
			ImageConfiguration config = {};
			m_Configuration.TargetImage = std::make_shared<VulkanImage>(config);
		}
		VkImageView attachments[1] = { std::dynamic_pointer_cast<VulkanImage>(m_Configuration.TargetImage)->GetVkImageView() };


		VkFramebufferCreateInfo framebufferCreateInfo = {};
		framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferCreateInfo.renderPass = std::dynamic_pointer_cast<VulkanRenderPass>(m_Configuration.TargetRenderPass)->GetVkRenderPass();
		framebufferCreateInfo.attachmentCount = 1;
		framebufferCreateInfo.pAttachments = attachments;
		framebufferCreateInfo.width = m_Configuration.Width;
		framebufferCreateInfo.height = m_Configuration.Height;
		framebufferCreateInfo.layers = 1;

		VK_CHECK(vkCreateFramebuffer(VulkanRenderContext::Get()->GetDevice()->GetVkDevice(), &framebufferCreateInfo, nullptr, &m_VkFramebuffer));
	}

	VulkanFramebuffer::~VulkanFramebuffer()
	{
		NB_PROFILE_FN();

		if (m_VkFramebuffer)
		{
			vkDestroyFramebuffer(VulkanRenderContext::Get()->GetDevice()->GetVkDevice(), m_VkFramebuffer, nullptr);
			m_VkFramebuffer = VK_NULL_HANDLE;
		}
	}
}