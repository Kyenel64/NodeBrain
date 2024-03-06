#pragma once

#include <vulkan/vulkan.h>

#include "GAPI/Vulkan/VulkanShader.h"
#include "GAPI/Vulkan/VulkanDevice.h"

namespace NodeBrain
{
	class VulkanPipelineLayout
	{
	public:
		VulkanPipelineLayout(std::shared_ptr<Shader> vertShader, std::shared_ptr<Shader> fragShader);
		virtual ~VulkanPipelineLayout();

	private:
		void Init();

	private:
		VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;

		std::shared_ptr<VulkanShader> m_VertexShader;
		std::shared_ptr<VulkanShader> m_FragmentShader;

		std::shared_ptr<VulkanDevice> m_Device;
	};
}