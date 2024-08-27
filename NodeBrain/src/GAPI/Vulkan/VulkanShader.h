#pragma once

#include <vulkan/vulkan.h>

#include "Renderer/Shader.h"
#include "GAPI/Vulkan/VulkanRenderContext.h"

class SpvReflectShaderModule;

namespace NodeBrain
{
	class VulkanShader : public Shader
	{
	public:
		VulkanShader(VulkanRenderContext& context, const std::filesystem::path& path);
		~VulkanShader() override;

		[[nodiscard]] const std::filesystem::path& GetShaderPath() const override { return m_ShaderPath; }
		[[nodiscard]] ShaderType GetShaderType() const override { return m_ShaderType; }
		[[nodiscard]] std::vector<LayoutBinding> GetLayout() const override { return m_Layout; }
		[[nodiscard]] VkShaderModule GetVkShaderModule() const { return m_VkShaderModule; }

	private:
		void Reflect(SpvReflectShaderModule& module);

	private:
		VulkanRenderContext& m_Context;
		
		VkShaderModule m_VkShaderModule = VK_NULL_HANDLE;
		const std::filesystem::path& m_ShaderPath;
		ShaderType m_ShaderType;

		std::vector<LayoutBinding> m_Layout;
	};
}
