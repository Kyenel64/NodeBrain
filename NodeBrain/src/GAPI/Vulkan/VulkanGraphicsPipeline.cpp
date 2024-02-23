#include "NBpch.h"
#include "VulkanGraphicsPipeline.h"

namespace NodeBrain
{
	VulkanGraphicsPipeline::VulkanGraphicsPipeline(std::shared_ptr<Shader> vertShader, std::shared_ptr<Shader> fragShader)
	{
		m_VertexShader = std::dynamic_pointer_cast<VulkanShader>(vertShader); // TODO: Check if this is ok.
		m_FragmentShader = std::dynamic_pointer_cast<VulkanShader>(fragShader);

		Init();
	}

	void VulkanGraphicsPipeline::Init()
	{
		NB_PROFILE_FN();

		// Vertex
		VkPipelineShaderStageCreateInfo vertShaderStageCreateInfo = {};
		vertShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageCreateInfo.module = m_VertexShader->GetVkShaderModule();
		vertShaderStageCreateInfo.pName = "main";

		// Fragment
		VkPipelineShaderStageCreateInfo fragShaderStageCreateInfo = {};
		fragShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageCreateInfo.module = m_FragmentShader->GetVkShaderModule();
		fragShaderStageCreateInfo.pName = "main";

		VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageCreateInfo, fragShaderStageCreateInfo };
	}
}