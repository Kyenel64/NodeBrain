#pragma once

#include <vulkan/vulkan.h>

#include "Renderer/GraphicsPipeline.h"
#include "GAPI/Vulkan/VulkanPipelineLayout.h"
#include "GAPI/Vulkan/VulkanRenderPass.h"

namespace NodeBrain
{
	class VulkanGraphicsPipeline : public GraphicsPipeline
	{
	public:
		VulkanGraphicsPipeline(std::shared_ptr<VulkanPipelineLayout> layout, std::shared_ptr<VulkanRenderPass> renderPass);
		virtual ~VulkanGraphicsPipeline();

		VkPipeline GetVkPipeline() const { return m_VkPipeline; }

	private:
		void Init();

	private:
		VkPipeline m_VkPipeline = VK_NULL_HANDLE;
		std::shared_ptr<VulkanDevice> m_Device;

		std::shared_ptr<VulkanPipelineLayout> m_PipelineLayout;
		std::shared_ptr<VulkanRenderPass> m_RenderPass;
	};
}