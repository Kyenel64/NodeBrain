#include "NBpch.h"
#include "VulkanDescriptorLayout.h"

#include "GAPI/Vulkan/VulkanRenderContext.h"

namespace NodeBrain
{
    void DescriptorLayoutBuilder::AddBinding(uint32_t binding, VkDescriptorType type)
    {
        VkDescriptorSetLayoutBinding newBinding = {};
        newBinding.binding = binding;
        newBinding.descriptorCount = 1;
        newBinding.descriptorType = type;

        m_Bindings.push_back(newBinding);
    }

    void DescriptorLayoutBuilder::Clear()
    {
        m_Bindings.clear();
    }

    VkDescriptorSetLayout DescriptorLayoutBuilder::Build(VkShaderStageFlags shaderStages)
    {
        for (auto& binding : m_Bindings)
            binding.stageFlags |= shaderStages;

        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
        descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptorSetLayoutCreateInfo.pNext = nullptr;
        descriptorSetLayoutCreateInfo.bindingCount = (uint32_t)m_Bindings.size();
        descriptorSetLayoutCreateInfo.pBindings = &m_Bindings[0];
        descriptorSetLayoutCreateInfo.flags = 0;

		VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
        VK_CHECK(vkCreateDescriptorSetLayout(VulkanRenderContext::Get()->GetDevice()->GetVkDevice(), &descriptorSetLayoutCreateInfo, nullptr, &descriptorSetLayout));

		return descriptorSetLayout;
    }



	void DescriptorAllocator::InitPool(uint32_t maxSets, std::span<PoolSizeRatio> poolRatios)
	{
        std::vector<VkDescriptorPoolSize> poolSizes;
        for (PoolSizeRatio ratio : poolRatios)
            poolSizes.push_back(VkDescriptorPoolSize{ .type = ratio.type, .descriptorCount = uint32_t(ratio.ratio * maxSets)});

        VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
        descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptorPoolCreateInfo.flags = 0;
        descriptorPoolCreateInfo.poolSizeCount = (uint32_t)poolSizes.size();
        descriptorPoolCreateInfo.pPoolSizes = &poolSizes[0];

        VK_CHECK(vkCreateDescriptorPool(VulkanRenderContext::Get()->GetDevice()->GetVkDevice(), &descriptorPoolCreateInfo, nullptr, &m_DescriptorPool));
	}

	void DescriptorAllocator::ClearDescriptors()
	{
		vkResetDescriptorPool(VulkanRenderContext::Get()->GetDevice()->GetVkDevice(), m_DescriptorPool, 0);
	}

	void DescriptorAllocator::DestroyPool()
	{
        vkDestroyDescriptorPool(VulkanRenderContext::Get()->GetDevice()->GetVkDevice(), m_DescriptorPool, nullptr);
	}

	VkDescriptorSet DescriptorAllocator::Allocate(VkDescriptorSetLayout descriptorSetLayout)
	{
        VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
        descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        descriptorSetAllocateInfo.descriptorPool = m_DescriptorPool;
        descriptorSetAllocateInfo.descriptorSetCount = 1;
        descriptorSetAllocateInfo.pSetLayouts = &descriptorSetLayout;

        VkDescriptorSet descriptorSet;
        VK_CHECK(vkAllocateDescriptorSets(VulkanRenderContext::Get()->GetDevice()->GetVkDevice(), &descriptorSetAllocateInfo, &descriptorSet));
        return descriptorSet;
	}
}