#pragma once

#include <vulkan/vulkan.h>

#include "Renderer/Shader.h"
#include "GAPI/Vulkan/VulkanRenderContext.h"

namespace NodeBrain
{
	class VulkanShader : public Shader
	{
	public:
		VulkanShader(const std::filesystem::path& path, ShaderType shaderType);
		virtual ~VulkanShader();

		virtual const std::filesystem::path& GetShaderPath() const override { return m_ShaderPath; }
		VkShaderModule GetVkShaderModule() const { return m_VkShaderModule; }

	private:
		VkShaderModule m_VkShaderModule = VK_NULL_HANDLE;
		const std::filesystem::path& m_ShaderPath;
		ShaderType m_ShaderType;
	};
}