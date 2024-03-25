#include "NBpch.h"
#include "VulkanShader.h"

#include "GAPI/Vulkan/VulkanRenderContext.h"
#include <Utils/FileUtils.h>

namespace NodeBrain
{
	VulkanShader::VulkanShader(const std::filesystem::path& path)
		: m_ShaderPath(path)
	{
		NB_PROFILE_FN();

		m_Device = VulkanRenderContext::Get()->GetDevice()->GetVkDevice();

		Init();
	}

	VulkanShader::~VulkanShader()
	{
		NB_PROFILE_FN();

		if (m_VkShaderModule)
			Destroy();
	}

	void VulkanShader::Destroy()
	{
		NB_PROFILE_FN();

		vkDestroyShaderModule(m_Device, m_VkShaderModule, nullptr);
		m_VkShaderModule = VK_NULL_HANDLE;
	}

	void VulkanShader::Init()
	{
		NB_PROFILE_FN();

		std::vector<char> buffer = Utils::ReadFile(m_ShaderPath);

		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = buffer.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(buffer.data());

		VK_CHECK(vkCreateShaderModule(m_Device, &createInfo, nullptr, &m_VkShaderModule));

		NB_INFO("Created shader module of size: {0}", buffer.size());
	}
}