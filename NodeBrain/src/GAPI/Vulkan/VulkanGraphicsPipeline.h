#pragma once

#include <vulkan/vulkan.h>

#include "GAPI/Vulkan/VulkanDevice.h"
#include "Renderer/GraphicsPipeline.h"

namespace NodeBrain
{
	class VulkanGraphicsPipeline : public GraphicsPipeline
	{
	public:
		VulkanGraphicsPipeline(const PipelineConfiguration& config);
		virtual ~VulkanGraphicsPipeline();

		virtual void SetPushConstantData(const void* buffer, uint32_t size, uint32_t offset) override;
		virtual void SetTargetImage(std::shared_ptr<Image> targetImage) override { m_Configuration.TargetImage = targetImage; }

		virtual const PipelineConfiguration& GetConfiguration() const override { return m_Configuration; }
		VkPipeline GetVkPipeline() const { return m_VkPipeline; }

	private:
		VkPipeline m_VkPipeline = VK_NULL_HANDLE;
		VkPipelineLayout m_VkPipelineLayout = VK_NULL_HANDLE;

		PipelineConfiguration m_Configuration;
	};
}