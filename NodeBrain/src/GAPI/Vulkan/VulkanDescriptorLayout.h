#pragma once

#include <span>

#include <vulkan/vulkan.h>

namespace NodeBrain
{
    struct DescriptorLayoutBuilder
    {
        std::vector<VkDescriptorSetLayoutBinding> m_Bindings;

		void AddBinding(uint32_t binding, VkDescriptorType type);
		void Clear();
		VkDescriptorSetLayout Build(VkShaderStageFlags shaderStages);

    };



	struct DescriptorAllocator
	{
		struct PoolSizeRatio
		{
			VkDescriptorType type;
			float ratio;
		};

		VkDescriptorPool m_DescriptorPool;
		
		void InitPool(uint32_t maxSets, std::span<PoolSizeRatio> poolRatios);
		void ClearDescriptors();
		void DestroyPool();

		VkDescriptorSet Allocate(VkDescriptorSetLayout descriptorSetLayout);
	};



	//class DescriptorSet
	//{
	//public:
	//	DescriptorSet();
	//
	//	void AddBinding(uint32_t binding, VkDescriptorType type);
	//
	//	void Build(VkShaderStageFlags shaderStages);
	//
	//private:
	//	VkDescriptorSetLayout m_VkDescriptorSetLayout = VK_NULL_HANDLE;
	//	VkDescriptorPool m_VkDescriptorPool = VK_NULL_HANDLE;
	//};
}