#pragma once

#include <vulkan/vulkan.h>

#include "Renderer/GraphicsPipeline.h"
#include "GAPI/Vulkan/VulkanRenderContext.h"

namespace NodeBrain
{
	class VulkanGraphicsPipeline : public GraphicsPipeline
	{
	public:
		VulkanGraphicsPipeline(VulkanRenderContext* context, const GraphicsPipelineConfiguration& config);
		virtual ~VulkanGraphicsPipeline();

		virtual void SetPushConstantData(const void* buffer, uint32_t size, uint32_t offset) override;
		virtual void SetTargetImage(std::shared_ptr<Image> targetImage) override { m_Configuration.TargetImage = targetImage; }

		virtual void BindDescriptorSet(std::shared_ptr<DescriptorSet> descriptorSet) override;

		virtual std::shared_ptr<Image> GetTargetImage() const override { return m_Configuration.TargetImage; }
		VkPipeline GetVkPipeline() const { return m_VkPipeline; }

	private:
		VulkanRenderContext* m_Context;
		
		VkPipeline m_VkPipeline = VK_NULL_HANDLE;
		VkPipelineLayout m_VkPipelineLayout = VK_NULL_HANDLE;

		GraphicsPipelineConfiguration m_Configuration;
	};
}