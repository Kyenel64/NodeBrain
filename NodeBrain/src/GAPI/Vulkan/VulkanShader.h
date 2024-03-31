#pragma once

#include <vulkan/vulkan.h>

#include "Renderer/Shader.h"

namespace NodeBrain
{
	class VulkanShader : public Shader
	{
	public:
		VulkanShader(const std::filesystem::path& path);
		virtual ~VulkanShader();

		void Destroy();

		virtual const std::filesystem::path& GetShaderPath() const override { return m_ShaderPath; }

		VkShaderModule GetVkShaderModule() const { return m_VkShaderModule; }

	private:
		VkShaderModule m_VkShaderModule = VK_NULL_HANDLE;
		VkDevice m_Device;
		const std::filesystem::path& m_ShaderPath;
	};
}