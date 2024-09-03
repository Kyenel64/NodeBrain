#include "NBpch.h"
#include "VulkanGraphicsPipeline.h"

#include "Core/Input.h"
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

			NB_ASSERT(false, "Invalid PolygonFillMode. fillMode must be a valid PolygonFillMode value.");
			return VK_POLYGON_MODE_MAX_ENUM;
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

			NB_ASSERT(false, "Invalid TopologyType. type must be a valid TopologyType value.");
			return VK_PRIMITIVE_TOPOLOGY_MAX_ENUM;
		}

		static VkFormat InputFormatToVkFormat(InputFormat format)
		{
			switch (format)
			{
			case InputFormat::None: return VK_FORMAT_UNDEFINED;
			case InputFormat::R32: return VK_FORMAT_R32_SFLOAT;
			case InputFormat::R32G32: return VK_FORMAT_R32G32_SFLOAT;
			case InputFormat::R32G32B32: return VK_FORMAT_R32G32B32_SFLOAT;
			case InputFormat::R32G32B32A32: return VK_FORMAT_R32G32B32A32_SFLOAT;
			default: return VK_FORMAT_UNDEFINED;
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

		uint32_t stride = 0;
		std::vector<VkVertexInputAttributeDescription> attributes;

		for (auto& inputVar : m_Configuration.VertexShader->GetInputVariables())
		{
			attributes.push_back({ inputVar.Location, 0, Utils::InputFormatToVkFormat(inputVar.Format), inputVar.Offset });
			switch (inputVar.Format)
			{
			case InputFormat::None: break;
			case InputFormat::R32: stride += 4; break;
			case InputFormat::R32G32: stride += 8; break;
			case InputFormat::R32G32B32: stride += 12; break;
			case InputFormat::R32G32B32A32: stride += 16; break;
			}
		}

		VkVertexInputBindingDescription bindingDescription = {};
		bindingDescription.binding = 0; // TODO
		bindingDescription.stride = stride;
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = {};
		vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
		vertexInputStateCreateInfo.pVertexBindingDescriptions = &bindingDescription; // TODO: Set buffer layout
		vertexInputStateCreateInfo.vertexAttributeDescriptionCount = 3;
		vertexInputStateCreateInfo.pVertexAttributeDescriptions = &attributes[0];

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
		rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
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
		std::vector<VkFormat> formats = { VK_FORMAT_R16G16B16A16_SFLOAT };
		pipelineRenderingCreateInfo.pColorAttachmentFormats = &formats[0];


		// --- Pipeline Layout ---
		VkPushConstantRange pushConstantRange = {};
		pushConstantRange.offset = 0;
		pushConstantRange.size = 128;
		pushConstantRange.stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS;

		std::unordered_map<uint32_t, std::vector<VkDescriptorSetLayoutBinding>> setLayoutBindings;
		std::vector<VkDescriptorSetLayout> setLayouts;
		for (auto& binding : m_Configuration.VertexShader->GetLayout())
		{
			VkDescriptorSetLayoutBinding setLayoutBinding = {};
			setLayoutBinding.binding = binding.Binding;
			setLayoutBinding.descriptorCount = binding.Count;
			setLayoutBinding.descriptorType = Utils::BindingTypeToVkDescriptorType(binding.Type);
			setLayoutBinding.stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS;
			setLayoutBindings[binding.Set].push_back(setLayoutBinding);
		}

		for (auto& binding : m_Configuration.FragmentShader->GetLayout())
		{
			// Check if there are duplicates from vertex shader
			bool duplicate = false;
			for (auto& existingBinding : setLayoutBindings[binding.Set])
			{
				if (existingBinding.binding == binding.Binding)
					duplicate = true;
			}

			if (!duplicate)
			{
				VkDescriptorSetLayoutBinding setLayoutBinding = {};
				setLayoutBinding.binding = binding.Binding;
				setLayoutBinding.descriptorCount = binding.Count;
				setLayoutBinding.descriptorType = Utils::BindingTypeToVkDescriptorType(binding.Type);
				setLayoutBinding.stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS;
				setLayoutBindings[binding.Set].push_back(setLayoutBinding);
			}
		}

		for (auto& [set, layoutBindings] : setLayoutBindings)
		{
			VkDescriptorBindingFlags flags = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT;
			VkDescriptorSetLayoutBindingFlagsCreateInfo flagsCreateInfo = {};
			flagsCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
			flagsCreateInfo.bindingCount = 1;
			flagsCreateInfo.pBindingFlags = &flags;

			VkDescriptorSetLayout layout = VK_NULL_HANDLE;
			VkDescriptorSetLayoutCreateInfo descriptorLayoutCreateInfo = {};
			descriptorLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			descriptorLayoutCreateInfo.bindingCount = (uint32_t)layoutBindings.size();
			descriptorLayoutCreateInfo.pBindings = &layoutBindings[0];
			descriptorLayoutCreateInfo.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT;
			descriptorLayoutCreateInfo.pNext = &flagsCreateInfo;
			VK_CHECK(vkCreateDescriptorSetLayout(m_Context.GetVkDevice(), &descriptorLayoutCreateInfo, nullptr, &layout));
			setLayouts.push_back(layout);
		}


		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
		pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutCreateInfo.setLayoutCount = (uint32_t)setLayouts.size();;
		pipelineLayoutCreateInfo.pSetLayouts = &setLayouts[0];
		pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
		pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;
		VK_CHECK(vkCreatePipelineLayout(m_Context.GetVkDevice(), &pipelineLayoutCreateInfo, nullptr, &m_VkPipelineLayout));

		// TODO: Shouldn't delete set layout.
		for (auto& descriptorLayout : setLayouts)
			vkDestroyDescriptorSetLayout(m_Context.GetVkDevice(), descriptorLayout, nullptr);

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

	void VulkanGraphicsPipeline::BindDescriptorSet(std::shared_ptr<DescriptorSet> descriptorSet, uint32_t index)
	{
		NB_PROFILE_FN();

		std::shared_ptr<VulkanDescriptorSet> vulkanSet = dynamic_pointer_cast<VulkanDescriptorSet>(descriptorSet);
		VkDescriptorSet vkDescriptorSet = vulkanSet->GetVkDescriptorSet();
		vkCmdBindDescriptorSets(m_Context.GetSwapchain().GetCurrentFrameData().CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_VkPipelineLayout, index, 1, &vkDescriptorSet, 0, nullptr);
	}
}
