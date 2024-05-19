#include "NBpch.h"
#include "VulkanShader.h"

#include "Utils/FileUtils.h"
#include "GAPI/Vulkan/VulkanRenderContext.h"

namespace NodeBrain
{
	VulkanShader::VulkanShader(const std::filesystem::path& path, ShaderType shaderType)
		: m_ShaderPath(path), m_ShaderType(shaderType)
	{
		NB_PROFILE_FN();

		std::vector<char> buffer = Utils::ReadFile(m_ShaderPath);

		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = buffer.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(buffer.data());
		VK_CHECK(vkCreateShaderModule(VulkanRenderContext::Get()->GetVkDevice(), &createInfo, nullptr, &m_VkShaderModule));
		NB_INFO("Created shader module of size: {0}", buffer.size());
	}

	VulkanShader::~VulkanShader()
	{
		NB_PROFILE_FN();

		vkDestroyShaderModule(VulkanRenderContext::Get()->GetVkDevice(), m_VkShaderModule, nullptr);
		m_VkShaderModule = VK_NULL_HANDLE;
	}
}