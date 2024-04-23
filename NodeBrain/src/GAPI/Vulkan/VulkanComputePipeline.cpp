#include "NBpch.h"
#include "VulkanComputePipeline.h"

#include "GAPI/Vulkan/VulkanRenderContext.h"

namespace NodeBrain
{
	VulkanComputePipeline::VulkanComputePipeline(std::shared_ptr<VulkanShader> computeShader)
		: m_ComputeShader(computeShader)
	{
		std::vector<VkDescriptorSetLayout> layouts = { computeShader->GetVkDescriptorSetLayout() };

		VkPipelineLayoutCreateInfo computeLayout{};
		computeLayout.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		computeLayout.pNext = nullptr;
		computeLayout.pSetLayouts = &layouts[0];
		computeLayout.setLayoutCount = 1;

		if (computeShader->GetPushConstantRange())
		{
			computeLayout.pPushConstantRanges = computeShader->GetPushConstantRange();
			computeLayout.pushConstantRangeCount = 1;
		}

		VK_CHECK(vkCreatePipelineLayout(VulkanRenderContext::Get()->GetVkDevice(), &computeLayout, nullptr, &m_VkPipelineLayout));

		VkPipelineShaderStageCreateInfo stageinfo{};
		stageinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		stageinfo.pNext = nullptr;
		stageinfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
		stageinfo.module = computeShader->GetVkShaderModule();
		stageinfo.pName = "main";

		VkComputePipelineCreateInfo computePipelineCreateInfo{};
		computePipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
		computePipelineCreateInfo.pNext = nullptr;
		computePipelineCreateInfo.layout = m_VkPipelineLayout;
		computePipelineCreateInfo.stage = stageinfo;
		
		VK_CHECK(vkCreateComputePipelines(VulkanRenderContext::Get()->GetVkDevice() , VK_NULL_HANDLE, 1, &computePipelineCreateInfo, nullptr, &m_VkPipeline));
	}

	VulkanComputePipeline::~VulkanComputePipeline()
	{
		vkDestroyPipelineLayout(VulkanRenderContext::Get()->GetVkDevice(), m_VkPipelineLayout, nullptr);
		vkDestroyPipeline(VulkanRenderContext::Get()->GetVkDevice(), m_VkPipeline, nullptr);
	}

	void VulkanComputePipeline::SetPushConstantData(const void* buffer, uint32_t size, uint32_t offset)
	{
		vkCmdPushConstants(VulkanRenderContext::Get()->GetSwapchain().GetCurrentFrameData().CommandBuffer, m_VkPipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, offset, size, buffer);
	}
}