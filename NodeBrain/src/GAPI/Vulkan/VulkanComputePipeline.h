#pragma once

#include <vulkan/vulkan.h>

#include "Renderer/ComputePipeline.h"
#include "GAPI/Vulkan/VulkanShader.h"

namespace NodeBrain
{
	class VulkanComputePipeline : public ComputePipeline
	{
	public:
		VulkanComputePipeline(std::shared_ptr<VulkanShader> computeShader);
		~VulkanComputePipeline();

		virtual void Bind() override;

		virtual void SetPushConstantData(const void* buffer, uint32_t size, uint32_t offset) override; 

		VkPipeline GetVkPipeline() const { return m_VkPipeline; }
		VkPipelineLayout GetVkPipelineLayout() const { return m_VkPipelineLayout; }
		std::shared_ptr<VulkanShader> GetComputeShader() const { return m_ComputeShader; }

	private:
		VkPipelineLayout m_VkPipelineLayout = VK_NULL_HANDLE;
		VkPipeline m_VkPipeline = VK_NULL_HANDLE;

		std::shared_ptr<VulkanShader> m_ComputeShader;
	};
}