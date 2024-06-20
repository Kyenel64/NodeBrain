#pragma once

#include <vulkan/vulkan.h>

#include "Renderer/Shader.h"
#include "GAPI/Vulkan/VulkanRenderContext.h"

namespace NodeBrain
{
	class VulkanShader : public Shader
	{
	public:
		VulkanShader(VulkanRenderContext& context, const std::filesystem::path& path, ShaderType shaderType);
		~VulkanShader() override;

		[[nodiscard]] const std::filesystem::path& GetShaderPath() const override { return m_ShaderPath; }
		[[nodiscard]] ShaderType GetShaderType() const override { return m_ShaderType; }
		[[nodiscard]] VkShaderModule GetVkShaderModule() const { return m_VkShaderModule; }

	private:
		VulkanRenderContext& m_Context;
		
		VkShaderModule m_VkShaderModule = VK_NULL_HANDLE;
		const std::filesystem::path& m_ShaderPath;
		ShaderType m_ShaderType;
	};
}