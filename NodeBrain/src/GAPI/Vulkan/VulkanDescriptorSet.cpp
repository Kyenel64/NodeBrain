#include "NBpch.h"
#include "VulkanDescriptorSet.h"

#include "GAPI/Vulkan/VulkanRenderContext.h"

namespace NodeBrain
{
	VulkanDescriptorSet::VulkanDescriptorSet(std::shared_ptr<VulkanDescriptorPool> pool, std::vector<VkDescriptorSetLayoutBinding> layoutBindings,  VkShaderStageFlags shaderStageFlags)
	{
		for (auto& binding : layoutBindings)
		    binding.stageFlags |= shaderStageFlags;
		
		VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
		descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptorSetLayoutCreateInfo.pNext = nullptr;
		descriptorSetLayoutCreateInfo.bindingCount = (uint32_t)layoutBindings.size();
		descriptorSetLayoutCreateInfo.pBindings = &layoutBindings[0];
		descriptorSetLayoutCreateInfo.flags = 0;
		
		VK_CHECK(vkCreateDescriptorSetLayout(VulkanRenderContext::Get()->GetDevice()->GetVkDevice(), &descriptorSetLayoutCreateInfo, nullptr, &m_VkDescriptorSetLayout));


		VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
        descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        descriptorSetAllocateInfo.descriptorPool = pool->GetVkDescriptorPool();
        descriptorSetAllocateInfo.descriptorSetCount = 1;
        descriptorSetAllocateInfo.pSetLayouts = &m_VkDescriptorSetLayout;

        VK_CHECK(vkAllocateDescriptorSets(VulkanRenderContext::Get()->GetDevice()->GetVkDevice(), &descriptorSetAllocateInfo, &m_VkDescriptorSet));
	}

	VulkanDescriptorSet::~VulkanDescriptorSet()
	{
		if (m_VkDescriptorSetLayout)
		{
			vkDestroyDescriptorSetLayout(VulkanRenderContext::Get()->GetDevice()->GetVkDevice(), m_VkDescriptorSetLayout, nullptr);
			m_VkDescriptorSetLayout = VK_NULL_HANDLE;
		}
	}

	void VulkanDescriptorSet::UpdateImageData(VkImageView imageView)
	{
		VkDescriptorImageInfo imgInfo{};
		imgInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
		imgInfo.imageView = imageView;

		VkWriteDescriptorSet drawImageWrite = {};
		drawImageWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		drawImageWrite.pNext = nullptr;

		drawImageWrite.dstBinding = 0;
		drawImageWrite.dstSet = m_VkDescriptorSet;
		drawImageWrite.descriptorCount = 1;
		drawImageWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		drawImageWrite.pImageInfo = &imgInfo;

		vkUpdateDescriptorSets(VulkanRenderContext::Get()->GetDevice()->GetVkDevice(), 1, &drawImageWrite, 0, nullptr);
	}
}