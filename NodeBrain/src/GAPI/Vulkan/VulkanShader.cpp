#include "NBpch.h"
#include "VulkanShader.h"

#include "GAPI/Vulkan/VulkanRenderContext.h"
#include <Utils/FileUtils.h>

namespace NodeBrain
{
	VulkanShader::VulkanShader(const std::filesystem::path& path)
	{
		NB_PROFILE_FN();

		m_Device = VulkanRenderContext::GetInstance()->GetDevice();

		std::vector<char> buffer = Utils::ReadFile(path);

		m_VkShaderModule = CreateShaderModule(buffer);
		NB_INFO("Created shader module of size: {0}", buffer.size());
	}

	VulkanShader::~VulkanShader()
	{
		NB_PROFILE_FN();

		vkDestroyShaderModule(m_Device->GetVkDevice(), m_VkShaderModule, nullptr);
	}

	VkShaderModule VulkanShader::CreateShaderModule(const std::vector<char>& buffer)
	{
		NB_PROFILE_FN();

		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = buffer.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(buffer.data());

		VkShaderModule shaderModule;
		VkResult result = vkCreateShaderModule(m_Device->GetVkDevice(), &createInfo, nullptr, &shaderModule);
		NB_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan shader module");

		return shaderModule;
	}
}