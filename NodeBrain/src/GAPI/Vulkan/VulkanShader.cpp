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

			NB_ASSERT(false, "Unavailable binding type");
			return VK_DESCRIPTOR_TYPE_MAX_ENUM;
		}

		static VkShaderStageFlags ShaderTypeToVkShaderStageFlags(ShaderType type)
		{
			switch (type)
			{
				case ShaderType::Vertex: return VK_SHADER_STAGE_VERTEX_BIT;
				case ShaderType::Fragment: return VK_SHADER_STAGE_FRAGMENT_BIT;
				case ShaderType::Compute: return VK_SHADER_STAGE_COMPUTE_BIT;
			}

			NB_ASSERT(false, "Unavailable shader type");
			return 0;
		}
	}


	VulkanShader::VulkanShader(const std::filesystem::path& path, ShaderType shaderType)
		: m_ShaderPath(path), m_ShaderType(shaderType), m_VkPushConstantRange({})
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

		if (m_VkDescriptorSetLayout)
		{
			vkDestroyDescriptorSetLayout(VulkanRenderContext::Get()->GetVkDevice(), m_VkDescriptorSetLayout, nullptr);
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
		
		VK_CHECK(vkCreateDescriptorSetLayout(VulkanRenderContext::Get()->GetVkDevice(), &descriptorSetLayoutCreateInfo, nullptr, &m_VkDescriptorSetLayout));
		
		// Create descriptor set
		std::vector<VkDescriptorSetLayout> setLayouts = { m_VkDescriptorSetLayout };
		VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
		descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		descriptorSetAllocateInfo.descriptorPool = VulkanRenderContext::Get()->GetVkDescriptorPool();
		descriptorSetAllocateInfo.descriptorSetCount = 1;
		descriptorSetAllocateInfo.pSetLayouts = &setLayouts[0];
		
		VK_CHECK(vkAllocateDescriptorSets(VulkanRenderContext::Get()->GetVkDevice(), &descriptorSetAllocateInfo, &m_VkDescriptorSet));
	}

	void VulkanShader::SetPushConstantLayout(uint32_t size, uint32_t offset)
	{
		VkShaderStageFlagBits stageFlag;
		switch (m_ShaderType)
		{
			case ShaderType::Compute: stageFlag = VK_SHADER_STAGE_COMPUTE_BIT; break;
			case ShaderType::Vertex: stageFlag = VK_SHADER_STAGE_VERTEX_BIT; break;
			case ShaderType::Fragment: stageFlag = VK_SHADER_STAGE_FRAGMENT_BIT; break;
		}

		m_VkPushConstantRange.offset = offset;
		m_VkPushConstantRange.size = size;
		m_VkPushConstantRange.stageFlags = stageFlag;
	}
}