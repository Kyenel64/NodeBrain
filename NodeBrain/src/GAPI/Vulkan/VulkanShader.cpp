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

		std::vector<char> buffer = Utils::ReadFile(m_ShaderPath);

		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = buffer.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(buffer.data());

		VK_CHECK(vkCreateShaderModule(VulkanRenderContext::Get()->GetDevice()->GetVkDevice(), &createInfo, nullptr, &m_VkShaderModule));
		NB_INFO("Created shader module of size: {0}", buffer.size());

		CreateDescriptorSets();
	}

	VulkanShader::~VulkanShader()
	{
		NB_PROFILE_FN();

		if (m_VkShaderModule)
		{
			vkDestroyShaderModule(VulkanRenderContext::Get()->GetDevice()->GetVkDevice(), m_VkShaderModule, nullptr);
			m_VkShaderModule = VK_NULL_HANDLE;
		}
	}

	void VulkanShader::CreateDescriptorSets()
	{
		// Temp
		VkDescriptorSetLayoutBinding newBinding = {};
        newBinding.binding = 0;
        newBinding.descriptorCount = 1;
        newBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		newBinding.stageFlags |= VK_SHADER_STAGE_COMPUTE_BIT;

		std::vector<VkDescriptorSetLayoutBinding> layoutBindings = { newBinding };
		
		VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
		descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptorSetLayoutCreateInfo.pNext = nullptr;
		descriptorSetLayoutCreateInfo.bindingCount = (uint32_t)layoutBindings.size();
		descriptorSetLayoutCreateInfo.pBindings = &layoutBindings[0];
		descriptorSetLayoutCreateInfo.flags = 0;
		
		VK_CHECK(vkCreateDescriptorSetLayout(VulkanRenderContext::Get()->GetDevice()->GetVkDevice(), &descriptorSetLayoutCreateInfo, nullptr, &m_VkDescriptorSetLayout));



		// --- Allocate descriptor set ---
		std::vector<VkDescriptorSetLayout> setLayouts = { m_VkDescriptorSetLayout };
		VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
        descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        descriptorSetAllocateInfo.descriptorPool = VulkanRenderContext::Get()->GetVkDescriptorPool();
        descriptorSetAllocateInfo.descriptorSetCount = 1;
        descriptorSetAllocateInfo.pSetLayouts = &setLayouts[0];

        VK_CHECK(vkAllocateDescriptorSets(VulkanRenderContext::Get()->GetDevice()->GetVkDevice(), &descriptorSetAllocateInfo, &m_VkDescriptorSet));
	}
}