#pragma once

#include <vulkan/vulkan.h>
#include <VMA/vk_mem_alloc.h>

#include "Renderer/Image.h"
#include "GAPI/Vulkan/VulkanRenderContext.h"

namespace NodeBrain
{
	class VulkanImage : public Image
	{
	public:
		VulkanImage(VulkanRenderContext* context, const ImageConfiguration& configuration);
		virtual ~VulkanImage();

		virtual uint64_t GetAddress() override;
		virtual const ImageConfiguration& GetConfiguration() const override { return m_Configuration; }

		VkImage GetVkImage() const { return m_VkImage[m_Context->GetSwapchain().GetFrameIndex()]; }
		VkImageView GetVkImageView() const { return m_VkImageView[m_Context->GetSwapchain().GetFrameIndex()]; }
		VkSampler GetVkSampler() const { return m_VkSampler[m_Context->GetSwapchain().GetFrameIndex()]; }
		
	private:
		VulkanRenderContext* m_Context;
		
		VkImage m_VkImage[FRAMES_IN_FLIGHT];
		VkImageView m_VkImageView[FRAMES_IN_FLIGHT];
		VkSampler m_VkSampler[FRAMES_IN_FLIGHT];
		VmaAllocation m_VmaAllocation[FRAMES_IN_FLIGHT];

		ImageConfiguration m_Configuration;

		uint64_t m_Address = 0;

	public:
		friend class VulkanDescriptorSet;
	};
}