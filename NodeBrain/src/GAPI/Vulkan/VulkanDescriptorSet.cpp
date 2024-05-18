#include "NBpch.h"
#include "VulkanDescriptorSet.h"

#include "GAPI/Vulkan/VulkanUtils.h"

#include "GAPI/Vulkan/VulkanUniformBuffer.h"

namespace NodeBrain
{
	VulkanDescriptorSet::VulkanDescriptorSet(const std::vector<LayoutBinding>& layout)
	{
		std::vector<VkDescriptorSetLayoutBinding> setLayoutbindings;
		for (const auto& binding : layout)
		{
			VkDescriptorSetLayoutBinding setLayoutBinding = {};
			setLayoutBinding.binding = binding.Binding;
			setLayoutBinding.descriptorCount = 1;
			setLayoutBinding.descriptorType = Utils::BindingTypeToVkDescriptorType(binding.Type);
			setLayoutBinding.stageFlags = VK_SHADER_STAGE_ALL;
			setLayoutbindings.push_back(setLayoutBinding);
		}
		VkDescriptorSetLayoutCreateInfo descriptorLayoutCreateInfo = {};
		descriptorLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptorLayoutCreateInfo.bindingCount = (uint32_t)setLayoutbindings.size();
		descriptorLayoutCreateInfo.pBindings = &setLayoutbindings[0];
		VK_CHECK(vkCreateDescriptorSetLayout(VulkanRenderContext::Get()->GetVkDevice(), &descriptorLayoutCreateInfo, nullptr, &m_VkDescriptorSetLayout));


		std::vector<VkDescriptorSetLayout> layouts(2, m_VkDescriptorSetLayout);
		VkDescriptorSetAllocateInfo descriptorAllocateInfo = {};
		descriptorAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		descriptorAllocateInfo.descriptorPool = VulkanRenderContext::Get()->GetVkDescriptorPool();
		descriptorAllocateInfo.descriptorSetCount = FRAMES_IN_FLIGHT;
		descriptorAllocateInfo.pSetLayouts = &layouts[0];
		VK_CHECK(vkAllocateDescriptorSets(VulkanRenderContext::Get()->GetVkDevice(), &descriptorAllocateInfo, &m_VkDescriptorSet[0]));
	}

	VulkanDescriptorSet::~VulkanDescriptorSet()
	{
		vkDestroyDescriptorSetLayout(VulkanRenderContext::Get()->GetVkDevice(), m_VkDescriptorSetLayout, nullptr);
		m_VkDescriptorSetLayout = VK_NULL_HANDLE;
	}

	void VulkanDescriptorSet::WriteBuffer(std::shared_ptr<UniformBuffer> buffer, uint32_t binding)
	{
		std::shared_ptr<VulkanUniformBuffer> vulkanUBO = std::static_pointer_cast<VulkanUniformBuffer>(buffer);

		for (size_t i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			VkDescriptorBufferInfo bufferInfo = {};
			bufferInfo.buffer = vulkanUBO->GetVkBuffer();
			bufferInfo.offset = 0;
			bufferInfo.range = vulkanUBO->GetSize();

			VkWriteDescriptorSet write = {};
			write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			write.dstBinding = binding;
			write.descriptorCount = 1;
			write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			write.dstSet = m_VkDescriptorSet[i];
			write.pBufferInfo = &bufferInfo;

			vkUpdateDescriptorSets(VulkanRenderContext::Get()->GetVkDevice(), 1, &write, 0, nullptr);
		}
	}

	void VulkanDescriptorSet::WriteImage(std::shared_ptr<Image> image, uint32_t binding)
	{
		std::shared_ptr<VulkanImage> vulkanImage = std::static_pointer_cast<VulkanImage>(image);

		for (size_t i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			VkDescriptorImageInfo imageinfo = {};
			imageinfo.imageView = vulkanImage->GetVkImageView();
			imageinfo.sampler = vulkanImage->GetVkSampler();
			imageinfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

			VkWriteDescriptorSet write = {};
			write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			write.dstBinding = binding;
			write.descriptorCount = 1;
			write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
			write.dstSet = m_VkDescriptorSet[i];
			write.pImageInfo = &imageinfo;

			vkUpdateDescriptorSets(VulkanRenderContext::Get()->GetVkDevice(), 1, &write, 0, nullptr);
		}
	}
}