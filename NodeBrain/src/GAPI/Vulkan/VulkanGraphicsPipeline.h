#pragma once

#include <vulkan/vulkan.h>

#include "GAPI/Vulkan/VulkanDevice.h"
#include "Renderer/GraphicsPipeline.h"
#include "Renderer/Framebuffer.h"

namespace NodeBrain
{
	class VulkanGraphicsPipeline : public GraphicsPipeline
	{
	public:
		VulkanGraphicsPipeline(const PipelineConfiguration& config);
		virtual ~VulkanGraphicsPipeline();

		virtual const PipelineConfiguration& GetConfiguration() const override { return m_Configuration; }
		VkPipeline GetVkPipeline() const { return m_VkPipeline; }

	private:
		VkPipeline m_VkPipeline = VK_NULL_HANDLE;
		VkPipelineLayout m_VkPipelineLayout = VK_NULL_HANDLE;

		PipelineConfiguration m_Configuration;
	};
}