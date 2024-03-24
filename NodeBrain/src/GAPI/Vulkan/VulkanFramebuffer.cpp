#include "NBpch.h"
#include "VulkanFramebuffer.h"

#include "GAPI/Vulkan/VulkanRenderContext.h"

namespace NodeBrain
{
	VulkanFramebuffer::VulkanFramebuffer(const FramebufferConfiguration& configuration)
		: m_Configuration(configuration)
	{
		Init();
	}

	VulkanFramebuffer::~VulkanFramebuffer()
	{
		NB_PROFILE_FN();

		if (m_VkFramebuffer)
		{
			vkDestroyFramebuffer(VulkanRenderContext::GetInstance()->GetDevice()->GetVkDevice(), m_VkFramebuffer, nullptr);
			m_VkFramebuffer = VK_NULL_HANDLE;
		}
	}

	void VulkanFramebuffer::Init()
	{
		NB_PROFILE_FN();
		
		// Create a new render pass if no render pass is provided
		if (!m_Configuration.RenderPass)
		{
			m_Configuration.RenderPass = std::make_shared<VulkanRenderPass>();
		}

		// Currently used for creating framebuffers from swapchain images
		if (!m_Configuration.Image)
		{
			ImageConfiguration config = {};
			m_Configuration.Image = std::make_shared<VulkanImage>(config);
		}
		VkImageView attachments[1] = { std::dynamic_pointer_cast<VulkanImage>(m_Configuration.Image)->GetVkImageView() };


		VkFramebufferCreateInfo framebufferCreateInfo = {};
		framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferCreateInfo.renderPass = std::dynamic_pointer_cast<VulkanRenderPass>(m_Configuration.RenderPass)->GetVkRenderPass();
		framebufferCreateInfo.attachmentCount = 1;
		framebufferCreateInfo.pAttachments = attachments;
		framebufferCreateInfo.width = m_Configuration.Width;
		framebufferCreateInfo.height = m_Configuration.Height;
		framebufferCreateInfo.layers = 1;

		VK_CHECK(vkCreateFramebuffer(VulkanRenderContext::GetInstance()->GetDevice()->GetVkDevice(), &framebufferCreateInfo, nullptr, &m_VkFramebuffer));
	}
}