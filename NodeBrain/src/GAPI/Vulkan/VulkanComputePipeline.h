#pragma once

#include <vulkan/vulkan.h>

#include "Renderer/ComputePipeline.h"

namespace NodeBrain
{
	class VulkanComputePipeline : public ComputePipeline
	{
	public:
		VulkanComputePipeline(const ComputePipelineConfiguration& configuration);
		virtual ~VulkanComputePipeline();

		virtual void SetPushConstantData(const void* buffer, uint32_t size, uint32_t offset) override; 
		virtual void SetTargetImage(std::shared_ptr<Image> targetImage) override { m_Configuration.TargetImage = targetImage; }

		virtual void BindDescriptorSet(std::shared_ptr<DescriptorSet> descriptorSet, uint32_t setIndex) override;

		virtual std::shared_ptr<Image> GetTargetImage() const override { return m_Configuration.TargetImage; }
		VkPipeline GetVkPipeline() const { return m_VkPipeline; }

	private:
		VkPipelineLayout m_VkPipelineLayout = VK_NULL_HANDLE;
		VkPipeline m_VkPipeline = VK_NULL_HANDLE;

		ComputePipelineConfiguration m_Configuration;
	};
}