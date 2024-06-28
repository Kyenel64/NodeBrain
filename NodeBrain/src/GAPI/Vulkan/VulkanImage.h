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
		VulkanImage(VulkanRenderContext& context, const ImageConfiguration& configuration);
		~VulkanImage() override;

		void Resize(uint32_t width, uint32_t height) override;
		void SetData(const void* data, uint32_t size) override;

		uint64_t GetAddress() override;
		[[nodiscard]] const ImageConfiguration& GetConfiguration() const override { return m_Configuration; }

		[[nodiscard]] VkImage GetVkImage() const { return m_VkImage[m_Context.GetSwapchain().GetFrameIndex()]; }
		[[nodiscard]] VkImageView GetVkImageView() const { return m_VkImageView[m_Context.GetSwapchain().GetFrameIndex()]; }
		[[nodiscard]] VkSampler GetVkSampler() const { return m_VkSampler[m_Context.GetSwapchain().GetFrameIndex()]; }

	private:
		VkResult CreateImage();
		void DestroyImage();
		
	private:
		VulkanRenderContext& m_Context;
		
		VkImage m_VkImage[FRAMES_IN_FLIGHT];
		VkImageView m_VkImageView[FRAMES_IN_FLIGHT];
		VkSampler m_VkSampler[FRAMES_IN_FLIGHT];
		VmaAllocation m_VmaAllocation[FRAMES_IN_FLIGHT];

		// Staging buffer
		VkBuffer m_VkStagingBuffer[FRAMES_IN_FLIGHT];
		VmaAllocation m_VmaStagingAllocation[FRAMES_IN_FLIGHT];
		void* m_StagingMappedData[FRAMES_IN_FLIGHT];

		ImageConfiguration m_Configuration;

		uint64_t m_Address = 0;

	public:
		friend class VulkanDescriptorSet;
	};
}