#include "NBpch.h"
#include "VulkanGraphicsPipeline.h"

#include "GAPI/Vulkan/VulkanShader.h"
#include "GAPI/Vulkan/VulkanDescriptorSet.h"

namespace NodeBrain
{
	namespace Utils
	{
		static VkPolygonMode PolygonFillModeToVkPolygonMode(PolygonFillMode fillMode)
		{
			switch (fillMode)
			{
				case PolygonFillMode::Point: return VK_POLYGON_MODE_POINT;
				case PolygonFillMode::Line: return VK_POLYGON_MODE_LINE;
				case PolygonFillMode::Fill: return VK_POLYGON_MODE_FILL;
			}
		}

		static VkPrimitiveTopology TopologyTypeToVkPrimitiveTopology(TopologyType type)
		{
			switch (type)
			{
				case TopologyType::PointList: return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
				case TopologyType::LineList: return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
				case TopologyType::LineStrip: return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
				case TopologyType::TriangleList: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
				case TopologyType::TriangleStrip: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
			}
		}
	}

	VulkanGraphicsPipeline::VulkanGraphicsPipeline(VulkanRenderContext& context, GraphicsPipelineConfiguration  configuration)
		: m_Context(context), m_Configuration(std::move(configuration))
	{
		NB_PROFILE_FN();

		NB_ASSERT(m_Configuration.VertexShader, "VertexShader null. Graphics pipeline must contain a valid vertex shader.");
		NB_ASSERT(m_Configuration.VertexShader->GetShaderType() == ShaderType::Vertex, "Shader type invalid. Graphics pipeline must contain a vertex shader.")

		NB_ASSERT(m_Configuration.FragmentShader, "FragmentShader null. Graphics pipeline must contain a valid fragment shader.");
		NB_ASSERT(m_Configuration.FragmentShader->GetShaderType() == ShaderType::Fragment, "Shader type invalid. Graphics pipeline must contain a fragment shader.")

		std::shared_ptr<VulkanShader> vertexShader = dynamic_pointer_cast<VulkanShader>(m_Configuration.VertexShader);
		std::shared_ptr<VulkanShader> fragShader = dynamic_pointer_cast<VulkanShader>(m_Configuration.FragmentShader);
		
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
		inputAssemblyStateCreateInfo.topology = Utils::TopologyTypeToVkPrimitiveTopology(m_Configuration.Topology);
		inputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

		// Rasterizer
		// TODO: parameterize
		VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo = {};
		rasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
		rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
		rasterizationStateCreateInfo.polygonMode = Utils::PolygonFillModeToVkPolygonMode(m_Configuration.FillMode);
		rasterizationStateCreateInfo.lineWidth = m_Configuration.LineWidth;
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

		// Rendering
		VkPipelineRenderingCreateInfo pipelineRenderingCreateInfo = {};
		pipelineRenderingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
		pipelineRenderingCreateInfo.colorAttachmentCount = 1;
		std::vector<VkFormat> formats = { VK_FORMAT_R16G16B16A16_SFLOAT, VK_FORMAT_B8G8R8A8_SRGB };
		pipelineRenderingCreateInfo.pColorAttachmentFormats = &formats[0];


		// --- Pipeline Layout ---
		VkPushConstantRange pushConstantRange = {};
		pushConstantRange.offset = 0;
		pushConstantRange.size = 128;
		pushConstantRange.stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS;

		std::vector<VkDescriptorSetLayout> layouts;
		for (auto& set : m_Configuration.GetDescriptorSets())
		{
			std::shared_ptr<VulkanDescriptorSet> vulkanSet = dynamic_pointer_cast<VulkanDescriptorSet>(set);
			layouts.push_back(vulkanSet->GetVkDescriptorSetLayout());
		}

		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
		pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutCreateInfo.setLayoutCount = (uint32_t)layouts.size();;
		pipelineLayoutCreateInfo.pSetLayouts = &layouts[0];
		pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
		pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;
		VK_CHECK(vkCreatePipelineLayout(m_Context.GetVkDevice(), &pipelineLayoutCreateInfo, nullptr, &m_VkPipelineLayout));


		// --- Pipeline ---
		VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
		pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineCreateInfo.pNext = &pipelineRenderingCreateInfo;
		pipelineCreateInfo.stageCount = 2;
		pipelineCreateInfo.pStages = shaderStages;
		pipelineCreateInfo.layout = m_VkPipelineLayout;

		pipelineCreateInfo.pVertexInputState = &vertexInputStateCreateInfo;
		pipelineCreateInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
		pipelineCreateInfo.pRasterizationState = &rasterizationStateCreateInfo;
		pipelineCreateInfo.pMultisampleState = &multisampleStateCreateInfo;
		pipelineCreateInfo.pDepthStencilState = nullptr; // TODO:
		pipelineCreateInfo.pColorBlendState = &colorBlendStateCreateInfo;
		pipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
		pipelineCreateInfo.pViewportState = &viewportStateCreateInfo;

		pipelineCreateInfo.renderPass = VK_NULL_HANDLE;
		pipelineCreateInfo.subpass = 0;
		pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
		pipelineCreateInfo.basePipelineIndex = -1; // Optional
		VK_CHECK(vkCreateGraphicsPipelines(m_Context.GetVkDevice(), VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &m_VkPipeline));
	}

	VulkanGraphicsPipeline::~VulkanGraphicsPipeline()
	{
		NB_PROFILE_FN();

		m_Context.WaitForGPU();

		vkDestroyPipelineLayout(m_Context.GetVkDevice(), m_VkPipelineLayout, nullptr);
		m_VkPipelineLayout = VK_NULL_HANDLE;
		
		vkDestroyPipeline(m_Context.GetVkDevice(), m_VkPipeline, nullptr);
		m_VkPipeline = VK_NULL_HANDLE;
	}

	void VulkanGraphicsPipeline::SetPushConstantData(const void* buffer, uint32_t size, uint32_t offset)
	{
		NB_PROFILE_FN();

		NB_ASSERT(buffer, "buffer null. Buffer must not be null.");
		NB_ASSERT(size, "size is 0. Size must be a non-zero value in bytes.");
		NB_ASSERT(size + offset <= 128, "Push constant overflow. Push constant offset and size must fit within the max push constant size: {0}.", 128);

		vkCmdPushConstants(m_Context.GetSwapchain().GetCurrentFrameData().CommandBuffer, m_VkPipelineLayout, VK_SHADER_STAGE_ALL_GRAPHICS, 0, 128, buffer);
	}

	void VulkanGraphicsPipeline::BindDescriptorSet(std::shared_ptr<DescriptorSet> descriptorSet)
	{
		NB_PROFILE_FN();

		const std::vector<std::shared_ptr<DescriptorSet>>& descriptorSets = m_Configuration.GetDescriptorSets();
		NB_ASSERT(std::find(descriptorSets.begin(), descriptorSets.end(), descriptorSet) != descriptorSets.end(), "Descriptor set not found. Descriptor set being bound must exist during pipeline creation.")

		uint32_t setIndex = std::find(descriptorSets.begin(), descriptorSets.end(), descriptorSet) - descriptorSets.begin();
		std::shared_ptr<VulkanDescriptorSet> vulkanSet = dynamic_pointer_cast<VulkanDescriptorSet>(descriptorSet);
		VkDescriptorSet vkDescriptorSet = vulkanSet->GetVkDescriptorSet();
		vkCmdBindDescriptorSets(m_Context.GetSwapchain().GetCurrentFrameData().CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_VkPipelineLayout, setIndex, 1, &vkDescriptorSet, 0, nullptr);
	}
}