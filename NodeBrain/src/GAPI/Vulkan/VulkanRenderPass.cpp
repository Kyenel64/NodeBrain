#include "NBpch.h"
#include "VulkanRenderPass.h"

#include "GAPI/Vulkan/VulkanRenderContext.h"

namespace NodeBrain
{
	VulkanRenderPass::VulkanRenderPass()
	{
		NB_PROFILE_FN();

		m_Device = VulkanRenderContext::GetInstance()->GetDevice();

		Init();
	}

	void VulkanRenderPass::Init()
	{
		NB_PROFILE_FN();

		VkAttachmentDescription colorAttachment = {};
		colorAttachment.format = VulkanRenderContext::GetInstance()->GetSwapchain()->GetFormat();
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;


		// Subpass
		VkAttachmentReference colorAttachmentRef = {};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;


		VkRenderPassCreateInfo renderPassCreateInfo = {};
		renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassCreateInfo.attachmentCount = 1;
		renderPassCreateInfo.pAttachments = &colorAttachment;
		renderPassCreateInfo.subpassCount = 1;
		renderPassCreateInfo.pSubpasses = &subpass;

		VkResult result = vkCreateRenderPass(m_Device->GetVkDevice(), &renderPassCreateInfo, nullptr, &m_VkRenderPass);
		NB_ASSERT(result == VK_SUCCESS, result);
	}

	VulkanRenderPass::~VulkanRenderPass()
	{
		NB_PROFILE_FN();

		vkDestroyRenderPass(m_Device->GetVkDevice(), m_VkRenderPass, nullptr);
	}
}