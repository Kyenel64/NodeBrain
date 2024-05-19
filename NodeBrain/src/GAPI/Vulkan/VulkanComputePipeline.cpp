#include "NBpch.h"
#include "VulkanComputePipeline.h"

#include "GAPI/Vulkan/VulkanShader.h"
#include "GAPI/Vulkan/VulkanRenderContext.h"
#include "GAPI/Vulkan/VulkanDescriptorSet.h"

namespace NodeBrain
{
	VulkanComputePipeline::VulkanComputePipeline(const ComputePipelineConfiguration& configuration)
		: m_Configuration(configuration)
	{
		NB_PROFILE_FN();

		std::shared_ptr<VulkanShader> computeShader = std::static_pointer_cast<VulkanShader>(m_Configuration.ComputeShader);

		// --- Pipeline Layout ---
		VkPushConstantRange pushConstantRange = {};
		pushConstantRange.offset = 0;
		pushConstantRange.size = 128;
		pushConstantRange.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

		std::vector<VkDescriptorSetLayout> layouts;
		for (auto& set : m_Configuration.GetDescriptorSets())
		{
			std::shared_ptr<VulkanDescriptorSet> vulkanSet = std::static_pointer_cast<VulkanDescriptorSet>(set);
			layouts.push_back(vulkanSet->GetVkDescriptorSetLayout());
		}
		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
		pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutCreateInfo.setLayoutCount = (uint32_t)layouts.size();;
		pipelineLayoutCreateInfo.pSetLayouts = &layouts[0];
		pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
		pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;
		VK_CHECK(vkCreatePipelineLayout(VulkanRenderContext::Get()->GetVkDevice(), &pipelineLayoutCreateInfo, nullptr, &m_VkPipelineLayout));


		// --- Pipeline ---
		VkPipelineShaderStageCreateInfo stageinfo{};
		stageinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		stageinfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
		stageinfo.module = computeShader->GetVkShaderModule();
		stageinfo.pName = "main";

		VkComputePipelineCreateInfo computePipelineCreateInfo{};
		computePipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
		computePipelineCreateInfo.stage = stageinfo;
		computePipelineCreateInfo.layout = m_VkPipelineLayout;
		VK_CHECK(vkCreateComputePipelines(VulkanRenderContext::Get()->GetVkDevice() , VK_NULL_HANDLE, 1, &computePipelineCreateInfo, nullptr, &m_VkPipeline));
	}

	VulkanComputePipeline::~VulkanComputePipeline()
	{
		NB_PROFILE_FN();

		vkDestroyPipelineLayout(VulkanRenderContext::Get()->GetVkDevice(), m_VkPipelineLayout, nullptr);
		m_VkPipelineLayout = VK_NULL_HANDLE;
		
		vkDestroyPipeline(VulkanRenderContext::Get()->GetVkDevice(), m_VkPipeline, nullptr);
		m_VkPipeline = VK_NULL_HANDLE;
	}

	void VulkanComputePipeline::SetPushConstantData(const void* buffer, uint32_t size, uint32_t offset)
	{
		NB_PROFILE_FN();

		vkCmdPushConstants(VulkanRenderContext::Get()->GetSwapchain().GetCurrentFrameData().CommandBuffer, m_VkPipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, offset, size, buffer);
	}

	void VulkanComputePipeline::BindDescriptorSet(std::shared_ptr<DescriptorSet> descriptorSet, uint32_t setIndex)
	{
		NB_PROFILE_FN();
		
		std::shared_ptr<VulkanDescriptorSet> vulkanSet = std::static_pointer_cast<VulkanDescriptorSet>(descriptorSet);
		VkDescriptorSet vkDescriptorSet = vulkanSet->GetVkDescriptorSet();
		vkCmdBindDescriptorSets(VulkanRenderContext::Get()->GetSwapchain().GetCurrentFrameData().CommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, m_VkPipelineLayout, setIndex, 1, &vkDescriptorSet, 0, nullptr);
	}
	
}