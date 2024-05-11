#include "NBpch.h"
#include "VulkanShader.h"

#include "GAPI/Vulkan/VulkanRenderContext.h"
#include <Utils/FileUtils.h>

#include "GAPI/Vulkan/VulkanUniformBuffer.h"

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
		for (auto& binding : layout)
		{
			VkDescriptorSetLayoutBinding descriptorSetLayoutBinding = {};
			descriptorSetLayoutBinding.binding = binding.Binding;
			descriptorSetLayoutBinding.descriptorCount = 1;
			descriptorSetLayoutBinding.descriptorType = Utils::BindingTypeToVkDescriptorType(binding.Type);
			descriptorSetLayoutBinding.stageFlags = Utils::ShaderTypeToVkShaderStageFlags(m_ShaderType);
			layoutBindings.push_back(descriptorSetLayoutBinding);
		}
		
		// Create descriptor layout
		VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
		descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptorSetLayoutCreateInfo.bindingCount = (uint32_t)layoutBindings.size();
		descriptorSetLayoutCreateInfo.pBindings = &layoutBindings[0];
		descriptorSetLayoutCreateInfo.flags = 0;
		
		VK_CHECK(vkCreateDescriptorSetLayout(VulkanRenderContext::Get()->GetVkDevice(), &descriptorSetLayoutCreateInfo, nullptr, &m_VkDescriptorSetLayout));
		
		// Create descriptor set
		std::vector<VkDescriptorSetLayout> setLayouts(FRAMES_IN_FLIGHT, m_VkDescriptorSetLayout);
		VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
		descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		descriptorSetAllocateInfo.descriptorPool = VulkanRenderContext::Get()->GetVkDescriptorPool();
		descriptorSetAllocateInfo.descriptorSetCount = FRAMES_IN_FLIGHT;
		descriptorSetAllocateInfo.pSetLayouts = &setLayouts[0];
		
		VK_CHECK(vkAllocateDescriptorSets(VulkanRenderContext::Get()->GetVkDevice(), &descriptorSetAllocateInfo, &m_VkDescriptorSets[0]));


		// Update descriptors
		for (auto& binding : layout)
		{
			if (binding.Type == BindingType::StorageImage)
			{
				for (size_t i = 0; i < FRAMES_IN_FLIGHT; i++)
				{
					std::shared_ptr<VulkanImage> vulkanImage = std::static_pointer_cast<VulkanImage>(binding.BindingImage);

					VkDescriptorImageInfo imgInfo = {};
					imgInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
					imgInfo.imageView = vulkanImage->GetVkImageView();

					VkWriteDescriptorSet drawImageWrite = {};
					drawImageWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					drawImageWrite.dstBinding = binding.Binding;
					drawImageWrite.dstSet = m_VkDescriptorSets[i];
					drawImageWrite.descriptorCount = 1;
					drawImageWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
					drawImageWrite.pImageInfo = &imgInfo;

					vkUpdateDescriptorSets(VulkanRenderContext::Get()->GetVkDevice(), 1, &drawImageWrite, 0, nullptr);
				}
			}
			else if (binding.Type == BindingType::UniformBuffer)
			{
				for (size_t i = 0; i < FRAMES_IN_FLIGHT; i++)
				{
					std::shared_ptr<VulkanUniformBuffer> vulkanBuffer = std::static_pointer_cast<VulkanUniformBuffer>(binding.BindingBuffer);

					VkDescriptorBufferInfo bufferInfo = {};
					bufferInfo.offset = 0;
					bufferInfo.range = vulkanBuffer->GetSize();
					bufferInfo.buffer = vulkanBuffer->GetVkBuffer();

					VkWriteDescriptorSet write = {};
					write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					write.dstBinding = 1;
					write.dstSet = m_VkDescriptorSets[i];
					write.descriptorCount = 1;
					write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
					write.pBufferInfo = &bufferInfo;

					vkUpdateDescriptorSets(VulkanRenderContext::Get()->GetVkDevice(), 1, &write, 0, nullptr);
				}
			}
		}
		
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