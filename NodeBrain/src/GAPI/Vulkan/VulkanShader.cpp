#include "NBpch.h"
#include "VulkanShader.h"

#include "GAPI/Vulkan/VulkanRenderContext.h"
#include <Utils/FileUtils.h>

namespace NodeBrain
{
	namespace Utils
	{
		static VkDescriptorType BindingTypeToVkDescriptorType(BindingType type)
		{
			switch (type)
			{
				case BindingType::StorageImage: return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
				case BindingType::UniformBuffer: return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			}
		}

		static VkShaderStageFlags ShaderTypeToVkShaderStageFlags(ShaderType type)
		{
			switch (type)
			{
				case ShaderType::Vertex: return VK_SHADER_STAGE_VERTEX_BIT;
				case ShaderType::Fragment: return VK_SHADER_STAGE_FRAGMENT_BIT;
				case ShaderType::Compute: return VK_SHADER_STAGE_COMPUTE_BIT;
			}
		}
	}


	VulkanShader::VulkanShader(const std::filesystem::path& path, ShaderType shaderType)
		: m_ShaderPath(path), m_ShaderType(shaderType)
	{
		NB_PROFILE_FN();

		std::vector<char> buffer = Utils::ReadFile(m_ShaderPath);

		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = buffer.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(buffer.data());

		VK_CHECK(vkCreateShaderModule(VulkanRenderContext::Get()->GetDevice()->GetVkDevice(), &createInfo, nullptr, &m_VkShaderModule));
		NB_INFO("Created shader module of size: {0}", buffer.size());
	}

	VulkanShader::~VulkanShader()
	{
		NB_PROFILE_FN();

		vkDestroyShaderModule(VulkanRenderContext::Get()->GetDevice()->GetVkDevice(), m_VkShaderModule, nullptr);
		m_VkShaderModule = VK_NULL_HANDLE;

		if (m_VkDescriptorSetLayout)
		{
			vkDestroyDescriptorSetLayout(VulkanRenderContext::Get()->GetDevice()->GetVkDevice(), m_VkDescriptorSetLayout, nullptr);
			m_VkDescriptorSetLayout = VK_NULL_HANDLE;
		}
	}

	void VulkanShader::SetLayout(const std::vector<LayoutBinding> layout)
	{
		// Create vulkan binding for each LayoutBinding.
		std::vector<VkDescriptorSetLayoutBinding> layoutBindings;
		for (size_t i = 0; i < layout.size(); i++)
		{
			VkDescriptorSetLayoutBinding binding = {};
			binding.binding = i;
			binding.descriptorCount = layout[i].Count;
			binding.descriptorType = Utils::BindingTypeToVkDescriptorType(layout[i].Type);
			binding.stageFlags |= Utils::ShaderTypeToVkShaderStageFlags(m_ShaderType);
			layoutBindings.push_back(binding);
		}
		
		// Create descriptor layout
		VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
		descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptorSetLayoutCreateInfo.pNext = nullptr;
		descriptorSetLayoutCreateInfo.bindingCount = (uint32_t)layoutBindings.size();
		descriptorSetLayoutCreateInfo.pBindings = &layoutBindings[0];
		descriptorSetLayoutCreateInfo.flags = 0;
		
		VK_CHECK(vkCreateDescriptorSetLayout(VulkanRenderContext::Get()->GetDevice()->GetVkDevice(), &descriptorSetLayoutCreateInfo, nullptr, &m_VkDescriptorSetLayout));
		
		// Create descriptor set
		std::vector<VkDescriptorSetLayout> setLayouts = { m_VkDescriptorSetLayout };
		VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
		descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		descriptorSetAllocateInfo.descriptorPool = VulkanRenderContext::Get()->GetVkDescriptorPool();
		descriptorSetAllocateInfo.descriptorSetCount = 1;
		descriptorSetAllocateInfo.pSetLayouts = &setLayouts[0];
		
		VK_CHECK(vkAllocateDescriptorSets(VulkanRenderContext::Get()->GetDevice()->GetVkDevice(), &descriptorSetAllocateInfo, &m_VkDescriptorSet));
	}
}