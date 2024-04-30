#include "NBpch.h"
#include "VulkanGraphicsPipeline.h"

#include "GAPI/Vulkan/VulkanRenderContext.h"
#include "GAPI/Vulkan/VulkanShader.h"

namespace NodeBrain
{
	VulkanGraphicsPipeline::VulkanGraphicsPipeline(const PipelineConfiguration& configuration)
		: m_Configuration(configuration)
	{
		NB_PROFILE_FN();

		std::shared_ptr<VulkanShader> vertexShader = std::dynamic_pointer_cast<VulkanShader>(m_Configuration.VertexShader);
		std::shared_ptr<VulkanShader> fragShader = std::dynamic_pointer_cast<VulkanShader>(m_Configuration.FragmentShader);
		
		// Vertex
		VkPipelineShaderStageCreateInfo vertShaderStageCreateInfo = {};
		vertShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageCreateInfo.module = vertexShader->GetVkShaderModule();
		vertShaderStageCreateInfo.pName = "main";

		// Fragment
		VkPipelineShaderStageCreateInfo fragShaderStageCreateInfo = {};
		fragShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageCreateInfo.module = fragShader->GetVkShaderModule();
		fragShaderStageCreateInfo.pName = "main";

		VkPipelineShaderStageCreateInfo shaderStages[2] = { vertShaderStageCreateInfo, fragShaderStageCreateInfo };

		// Vertex Input
		VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = {};
		vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputStateCreateInfo.vertexBindingDescriptionCount = 0;
		vertexInputStateCreateInfo.pVertexBindingDescriptions = nullptr; // TODO: Set buffer layout
		vertexInputStateCreateInfo.vertexAttributeDescriptionCount = 0;
		vertexInputStateCreateInfo.pVertexAttributeDescriptions = nullptr;

		// Input Assembly
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo = {};
		inputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

		// Rasterizer
		// TODO: parameterize
		VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo = {};
		rasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
		rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
		rasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizationStateCreateInfo.lineWidth = 1.0f;
		rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
		rasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;

		// Multisampling
		VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo = {};
		multisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampleStateCreateInfo.sampleShadingEnable = VK_FALSE; // TEMP: Disable multisampling for now
		multisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

		// Depth & Stencil Testing
		// TODO: Implement

		// Color Blending
		// TODO: Implement
		VkPipelineColorBlendAttachmentState colorBlendAttachmentState = {};
		colorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachmentState.blendEnable = VK_TRUE;
		colorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		colorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		colorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;

		VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo = {};
		colorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
		colorBlendStateCreateInfo.attachmentCount = 1;
		colorBlendStateCreateInfo.pAttachments = &colorBlendAttachmentState;

		// Dynamic states
		std::vector<VkDynamicState> dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
		VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = {};
		dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
		dynamicStateCreateInfo.pDynamicStates = &dynamicStates[0];

		// Viewport dynamic state
		VkPipelineViewportStateCreateInfo viewportStateCreateInfo{};
		viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportStateCreateInfo.viewportCount = 1;
		viewportStateCreateInfo.scissorCount = 1;

		// Push constants
		uint32_t pushConstantCount = 0;
		VkPushConstantRange pushConstantRanges[2] = {};
		if (vertexShader->GetPushConstantRange().size)
		{
			pushConstantRanges[pushConstantCount] = vertexShader->GetPushConstantRange();
			pushConstantCount++;
		}
		if (fragShader->GetPushConstantRange().size)
		{
			pushConstantRanges[pushConstantCount] = fragShader->GetPushConstantRange();
			pushConstantCount++;
		}

		// Pipeline layout
		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
		pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutCreateInfo.setLayoutCount = 0;
		pipelineLayoutCreateInfo.pSetLayouts = nullptr;
		pipelineLayoutCreateInfo.pushConstantRangeCount = pushConstantCount;
		pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRanges[0];

		VK_CHECK(vkCreatePipelineLayout(VulkanRenderContext::Get()->GetVkDevice(), &pipelineLayoutCreateInfo, nullptr, &m_VkPipelineLayout));


		VkPipelineRenderingCreateInfo pipelineRenderingCreateInfo = {};
		pipelineRenderingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
		pipelineRenderingCreateInfo.colorAttachmentCount = 1;
		VkFormat format = VK_FORMAT_R16G16B16A16_SFLOAT; // temp
		pipelineRenderingCreateInfo.pColorAttachmentFormats = &format;

		VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
		pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineCreateInfo.pNext = &pipelineRenderingCreateInfo;
		pipelineCreateInfo.stageCount = 2;
		pipelineCreateInfo.pStages = shaderStages;

		pipelineCreateInfo.pVertexInputState = &vertexInputStateCreateInfo;
		pipelineCreateInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
		pipelineCreateInfo.pRasterizationState = &rasterizationStateCreateInfo;
		pipelineCreateInfo.pMultisampleState = &multisampleStateCreateInfo;
		pipelineCreateInfo.pDepthStencilState = nullptr; // TODO:
		pipelineCreateInfo.pColorBlendState = &colorBlendStateCreateInfo;
		pipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
		pipelineCreateInfo.pViewportState = &viewportStateCreateInfo;

		pipelineCreateInfo.layout = m_VkPipelineLayout;
		pipelineCreateInfo.renderPass = VK_NULL_HANDLE;
		pipelineCreateInfo.subpass = 0;
		pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
		pipelineCreateInfo.basePipelineIndex = -1; // Optional

		VK_CHECK(vkCreateGraphicsPipelines(VulkanRenderContext::Get()->GetVkDevice(), VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &m_VkPipeline));
	}

	VulkanGraphicsPipeline::~VulkanGraphicsPipeline()
	{
		NB_PROFILE_FN();

		vkDestroyPipelineLayout(VulkanRenderContext::Get()->GetVkDevice(), m_VkPipelineLayout, nullptr);
		vkDestroyPipeline(VulkanRenderContext::Get()->GetVkDevice(), m_VkPipeline, nullptr);
	}

	void VulkanGraphicsPipeline::SetPushConstantData(const void* buffer, uint32_t size, uint32_t offset)
	{
		vkCmdPushConstants(VulkanRenderContext::Get()->GetSwapchain().GetCurrentFrameData().CommandBuffer, m_VkPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, offset, size, buffer);
	}
}