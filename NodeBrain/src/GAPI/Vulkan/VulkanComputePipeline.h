#pragma once

#include <vulkan/vulkan.h>

#include "Renderer/ComputePipeline.h"
#include "GAPI/Vulkan/VulkanRenderContext.h"

namespace NodeBrain
{
	class VulkanComputePipeline : public ComputePipeline
	{
	public:
		VulkanComputePipeline(VulkanRenderContext& context, ComputePipelineConfiguration configuration);
		~VulkanComputePipeline() override;

		void SetPushConstantData(const void* buffer, uint32_t size, uint32_t offset) override;
		void SetTargetFramebuffer(std::shared_ptr<Framebuffer> targetFramebuffer) override { m_Configuration.TargetFramebuffer = targetFramebuffer; }

		void BindDescriptorSet(std::shared_ptr<DescriptorSet> descriptorSet) override;

		[[nodiscard]] std::shared_ptr<Framebuffer> GetTargetFramebuffer() const override { return m_Configuration.TargetFramebuffer; }
		[[nodiscard]] VkPipeline GetVkPipeline() const { return m_VkPipeline; }

	private:
		VulkanRenderContext& m_Context;
		
		VkPipelineLayout m_VkPipelineLayout = VK_NULL_HANDLE;
		VkPipeline m_VkPipeline = VK_NULL_HANDLE;

		ComputePipelineConfiguration m_Configuration;
	};
}