#include "NBpch.h"
#include "VulkanDescriptorSet.h"

#include "GAPI/Vulkan/VulkanUtils.h"
#include "GAPI/Vulkan/VulkanUniformBuffer.h"
#include "GAPI/Vulkan/VulkanTexture2D.h"

namespace NodeBrain
{
	VulkanDescriptorSet::VulkanDescriptorSet(VulkanRenderContext& context, const std::vector<LayoutBinding>& layout)
		: m_Context(context), m_Layout(layout)
	{
		NB_PROFILE_FN();

		NB_ASSERT(!m_Layout.empty(), "layout contains 0 elements. Descriptor set must be created with a valid layout.");

		// --- Descriptor Layout ---
		std::vector<VkDescriptorSetLayoutBinding> setLayoutbindings;
		for (const auto& binding : m_Layout)
		{
			VkDescriptorSetLayoutBinding setLayoutBinding = {};
			setLayoutBinding.binding = binding.Binding;
			setLayoutBinding.descriptorCount = binding.Count;
			setLayoutBinding.descriptorType = Utils::BindingTypeToVkDescriptorType(binding.Type);
			setLayoutBinding.stageFlags = VK_SHADER_STAGE_ALL;
			setLayoutbindings.push_back(setLayoutBinding);
		}
		VkDescriptorSetLayoutCreateInfo descriptorLayoutCreateInfo = {};
		descriptorLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptorLayoutCreateInfo.bindingCount = (uint32_t)setLayoutbindings.size();
		descriptorLayoutCreateInfo.pBindings = &setLayoutbindings[0];
		VK_CHECK(vkCreateDescriptorSetLayout(m_Context.GetVkDevice(), &descriptorLayoutCreateInfo, nullptr, &m_VkDescriptorSetLayout));


		// --- Descriptor Set ---
		std::vector<VkDescriptorSetLayout> layouts(FRAMES_IN_FLIGHT, m_VkDescriptorSetLayout);
		VkDescriptorSetAllocateInfo descriptorAllocateInfo = {};
		descriptorAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		descriptorAllocateInfo.descriptorPool = m_Context.GetVkDescriptorPool();
		descriptorAllocateInfo.descriptorSetCount = FRAMES_IN_FLIGHT;
		descriptorAllocateInfo.pSetLayouts = &layouts[0];
		VK_CHECK(vkAllocateDescriptorSets(m_Context.GetVkDevice(), &descriptorAllocateInfo, &m_VkDescriptorSet[0]));
	}

	VulkanDescriptorSet::~VulkanDescriptorSet()
	{
		NB_PROFILE_FN();

		m_Context.WaitForGPU();

		vkDestroyDescriptorSetLayout(m_Context.GetVkDevice(), m_VkDescriptorSetLayout, nullptr);
		m_VkDescriptorSetLayout = VK_NULL_HANDLE;
	}

	void VulkanDescriptorSet::WriteBuffer(const std::shared_ptr<UniformBuffer>& buffer, uint32_t binding, uint32_t size, uint32_t offset)
	{
		NB_PROFILE_FN();

		NB_ASSERT(buffer, "Invalid uniform buffer");
		bool isDynamic = false;
		for (auto& layout : m_Layout)
		{
			if (layout.Binding == binding)
			{
				if (layout.Type == BindingType::UniformBufferDynamic)
					isDynamic = true;
				NB_ASSERT(layout.Type == BindingType::UniformBuffer || layout.Type == BindingType::UniformBufferDynamic,
					"Invalid binding type at index {0}. Binding must be of type UniformBuffer.", binding);
			}
		}

		std::shared_ptr<VulkanUniformBuffer> vulkanUBO = dynamic_pointer_cast<VulkanUniformBuffer>(buffer);

		VkDescriptorBufferInfo bufferInfo = {};
		bufferInfo.buffer = vulkanUBO->GetVkBuffer();
		bufferInfo.offset = offset;
		bufferInfo.range = size;

		VkWriteDescriptorSet write = {};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.dstBinding = binding;
		write.descriptorCount = 1;
			write.descriptorType = isDynamic ? VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC : VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		write.pBufferInfo = &bufferInfo;

		if (m_Context.IsInRuntime())
		{
			write.dstSet = m_VkDescriptorSet[m_Context.GetSwapchain().GetFrameIndex()];
			vkUpdateDescriptorSets(m_Context.GetVkDevice(), 1, &write, 0, nullptr);
		}
		else
		{
			for (size_t i = 0; i < FRAMES_IN_FLIGHT; i++)
			{
				write.dstSet = m_VkDescriptorSet[i];
				vkUpdateDescriptorSets(m_Context.GetVkDevice(), 1, &write, 0, nullptr);
			}
		}

	}

	void VulkanDescriptorSet::WriteImage(const std::shared_ptr<Texture2D>& texture, uint32_t binding)
	{
		NB_PROFILE_FN();

		NB_ASSERT(texture, "Invalid texture");
		for (auto& layout : m_Layout)
		{
			if (layout.Binding == binding)
				NB_ASSERT(layout.Type == BindingType::StorageImage, "Invalid binding type at index {0}. Binding must be of type StorageImage.", binding);
		}
		
		std::shared_ptr<VulkanTexture2D> vulkanTexture = dynamic_pointer_cast<VulkanTexture2D>(texture);

		VkDescriptorImageInfo imageinfo = {};
		imageinfo.imageView = vulkanTexture->GetVkImageView();
		imageinfo.sampler = vulkanTexture->GetVkSampler();
		imageinfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

		VkWriteDescriptorSet write = {};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.dstBinding = binding;
		write.descriptorCount = 1;
		write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		write.pImageInfo = &imageinfo;

		if (m_Context.IsInRuntime())
		{
			write.dstSet = m_VkDescriptorSet[m_Context.GetSwapchain().GetFrameIndex()];
			vkUpdateDescriptorSets(m_Context.GetVkDevice(), 1, &write, 0, nullptr);
		}
		else
		{
			for (size_t i = 0; i < FRAMES_IN_FLIGHT; i++)
			{
				write.dstSet = m_VkDescriptorSet[i];
				vkUpdateDescriptorSets(m_Context.GetVkDevice(), 1, &write, 0, nullptr);
			}
		}

	}

	void VulkanDescriptorSet::WriteSampler(const std::shared_ptr<Texture2D>& texture, uint32_t binding)
	{
		NB_PROFILE_FN();

		NB_ASSERT(texture, "Invalid texture");
		for (auto& layout : m_Layout)
		{
			if (layout.Binding == binding)
			NB_ASSERT(layout.Type == BindingType::ImageSampler, "Invalid binding type. Binding must be of type ImageSampler.");
		}

		const std::shared_ptr<VulkanTexture2D>& vulkanTexture = dynamic_pointer_cast<VulkanTexture2D>(texture);

		VkDescriptorImageInfo imageinfo = {};
		imageinfo.imageView = vulkanTexture->GetVkImageView();
		imageinfo.sampler = vulkanTexture->GetVkSampler();
		imageinfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		VkWriteDescriptorSet write = {};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.dstBinding = binding;
		write.descriptorCount = 1;
		write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		write.pImageInfo = &imageinfo;

		if (m_Context.IsInRuntime())
		{
			write.dstSet = m_VkDescriptorSet[m_Context.GetSwapchain().GetFrameIndex()];
			vkUpdateDescriptorSets(m_Context.GetVkDevice(), 1, &write, 0, nullptr);
		}
		else
		{
			for (size_t i = 0; i < FRAMES_IN_FLIGHT; i++)
			{
				write.dstSet = m_VkDescriptorSet[i];
				vkUpdateDescriptorSets(m_Context.GetVkDevice(), 1, &write, 0, nullptr);
			}
		}
	}

	void VulkanDescriptorSet::WriteSamplers(const std::vector<std::shared_ptr<Texture2D>>& textures, uint32_t binding)
	{
		NB_PROFILE_FN();

		NB_ASSERT(!textures.empty(), "textures is empty.");
		for (auto& layout : m_Layout)
		{
			if (layout.Binding == binding)
				NB_ASSERT(layout.Type == BindingType::ImageSampler, "Invalid binding type. Binding must be of type ImageSampler.");
		}

		std::vector<VkDescriptorImageInfo> imageInfos;
		for (auto& texture : textures)
		{
			const std::shared_ptr<VulkanTexture2D>& vulkanTexture = dynamic_pointer_cast<VulkanTexture2D>(texture);

			VkDescriptorImageInfo imageInfo = {};
			imageInfo.imageView = vulkanTexture->GetVkImageView();
			imageInfo.sampler = vulkanTexture->GetVkSampler();
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

			imageInfos.push_back(imageInfo);
		}

		VkWriteDescriptorSet write = {};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.dstBinding = binding;
		write.descriptorCount = textures.size();
		write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		write.pImageInfo = &imageInfos[0];

		if (m_Context.IsInRuntime())
		{
			write.dstSet = m_VkDescriptorSet[m_Context.GetSwapchain().GetFrameIndex()];
			vkUpdateDescriptorSets(m_Context.GetVkDevice(), 1, &write, 0, nullptr);
		}
		else
		{
			for (size_t i = 0; i < FRAMES_IN_FLIGHT; i++)
			{
				write.dstSet = m_VkDescriptorSet[i];
				vkUpdateDescriptorSets(m_Context.GetVkDevice(), 1, &write, 0, nullptr);
			}
		}
	}
}
