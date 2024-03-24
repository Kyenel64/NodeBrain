#include "NBpch.h"
#include "VulkanRenderPass.h"

#include "GAPI/Vulkan/VulkanRenderContext.h"

namespace NodeBrain
{
	VulkanRenderPass::VulkanRenderPass()
	{
		NB_PROFILE_FN();

		Init();
	}

	void VulkanRenderPass::Init()
	{
		NB_PROFILE_FN();

		VkAttachmentDescription colorAttachment = {};
		colorAttachment.format = VK_FORMAT_B8G8R8A8_SRGB; // temp. parametrize
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

		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;


		VkRenderPassCreateInfo renderPassCreateInfo = {};
		renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassCreateInfo.attachmentCount = 1;
		renderPassCreateInfo.pAttachments = &colorAttachment;
		renderPassCreateInfo.subpassCount = 1;
		renderPassCreateInfo.pSubpasses = &subpass;
		renderPassCreateInfo.dependencyCount = 1;
		renderPassCreateInfo.pDependencies = &dependency;

		VK_CHECK(vkCreateRenderPass(VulkanRenderContext::GetInstance()->GetDevice()->GetVkDevice(), &renderPassCreateInfo, nullptr, &m_VkRenderPass));
	}

	VulkanRenderPass::~VulkanRenderPass()
	{
		NB_PROFILE_FN();

		m_TargetFramebuffer = nullptr;

		if (m_VkRenderPass)
		{
			vkDestroyRenderPass(VulkanRenderContext::GetInstance()->GetDevice()->GetVkDevice(), m_VkRenderPass, nullptr);
			m_VkRenderPass = VK_NULL_HANDLE;
		}
	}
}