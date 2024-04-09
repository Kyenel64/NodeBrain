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

		virtual const std::filesystem::path& GetShaderPath() const override { return m_ShaderPath; }
		VkShaderModule GetVkShaderModule() const { return m_VkShaderModule; }

		VkDescriptorSetLayout GetVkDescriptorSetLayout() const { return m_VkDescriptorSetLayout; }
		VkDescriptorSet GetVkDescriptorSet() const { return m_VkDescriptorSet; }

	private:
		void CreateDescriptorSets();
		

	private:
		VkShaderModule m_VkShaderModule = VK_NULL_HANDLE;
		const std::filesystem::path& m_ShaderPath;

		VkDescriptorSetLayout m_VkDescriptorSetLayout = VK_NULL_HANDLE;
		VkDescriptorSet m_VkDescriptorSet = VK_NULL_HANDLE;
	};
}