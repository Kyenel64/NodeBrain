#include "NBpch.h"
#include "VulkanGraphicsPipeline.h"

#include "VulkanRenderContext.h"

namespace NodeBrain
{
	VulkanGraphicsPipeline::VulkanGraphicsPipeline(std::shared_ptr<VulkanPipelineLayout> layout, std::shared_ptr<VulkanRenderPass> renderPass)
		: m_PipelineLayout(layout), m_RenderPass(renderPass)
	{
		NB_PROFILE_FN();
		
		m_Device = VulkanRenderContext::GetInstance()->GetDevice();

		Init();
	}

	void VulkanGraphicsPipeline::Init()
	{
		NB_PROFILE_FN();

		const VulkanPipelineCreateInfos& createInfos = m_PipelineLayout->GetCreateInfos();

		VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
		pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineCreateInfo.stageCount = 2;
		pipelineCreateInfo.pStages = createInfos.ShaderStages;

		pipelineCreateInfo.pVertexInputState = &createInfos.VertexInput;
		pipelineCreateInfo.pInputAssemblyState = &createInfos.InputAssembly;
		pipelineCreateInfo.pRasterizationState = &createInfos.Rasterizer;
		pipelineCreateInfo.pMultisampleState = &createInfos.Multisampling;
		pipelineCreateInfo.pDepthStencilState = nullptr; // TODO:
		pipelineCreateInfo.pColorBlendState = &createInfos.ColorBlend;
		pipelineCreateInfo.pDynamicState = &createInfos.DynamicState;
		pipelineCreateInfo.pViewportState = &createInfos.ViewportState;

		pipelineCreateInfo.layout = m_PipelineLayout->GetVkPipelineLayout();

		pipelineCreateInfo.renderPass = m_RenderPass->GetVkRenderPass();
		pipelineCreateInfo.subpass = 0;
		pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
		pipelineCreateInfo.basePipelineIndex = -1; // Optional

		VkResult result = vkCreateGraphicsPipelines(m_Device->GetVkDevice(), VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &m_VkGraphicsPipeline);
		NB_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan graphics pipeline");

	}

	VulkanGraphicsPipeline::~VulkanGraphicsPipeline()
	{
		NB_PROFILE_FN();

		vkDestroyPipeline(m_Device->GetVkDevice(), m_VkGraphicsPipeline, nullptr);
	}
}