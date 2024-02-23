#pragma once

#include <vulkan/vulkan.h>

#include "GAPI/Vulkan/VulkanDevice.h"
#include "Renderer/Shader.h"

namespace NodeBrain
{
	class VulkanShader : public Shader
	{
	public:
		VulkanShader(const std::filesystem::path& path);
		virtual ~VulkanShader();

		VkShaderModule GetVkShaderModule() const { return m_VkShaderModule; }

	private:
		VkShaderModule CreateShaderModule(const std::vector<char>& buffer);

	private:
		VkShaderModule m_VkShaderModule = VK_NULL_HANDLE;
		std::shared_ptr<VulkanDevice> m_Device;
	};
}