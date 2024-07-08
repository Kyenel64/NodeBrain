#pragma once

#include "Renderer/Texture2D.h"
#include "GAPI/Vulkan/VulkanRenderContext.h"

namespace NodeBrain
{
	class VulkanTexture2D : public Texture2D
	{
	public:
		VulkanTexture2D(VulkanRenderContext& context, const Texture2DConfiguration& configuration);
		VulkanTexture2D(VulkanRenderContext& context, std::filesystem::path path);
		~VulkanTexture2D() override;

		void SetData(const void* data, uint32_t size) override;

		// Pointer address is equal to VkDescriptorSet. Can only be accessed by ImGui.
		uint64_t GetAddress() override;
		[[nodiscard]] const Texture2DConfiguration& GetConfiguration() const override { return m_Configuration; }

		[[nodiscard]] VkImage GetVkImage() const { return m_VkImage[m_Context.GetSwapchain().GetFrameIndex()]; }
		[[nodiscard]] VkImageView GetVkImageView() const { return m_VkImageView[m_Context.GetSwapchain().GetFrameIndex()]; }
		[[nodiscard]] VkSampler GetVkSampler() const { return m_VkSampler[m_Context.GetSwapchain().GetFrameIndex()]; }

	private:
		VkResult Create(uint32_t width, uint32_t height, ImageFormat format);
		void Destroy();
		

	private:
		VulkanRenderContext& m_Context;

		Texture2DConfiguration m_Configuration;
		std::filesystem::path m_Path;

		VkImage m_VkImage[FRAMES_IN_FLIGHT];
		VkImageView m_VkImageView[FRAMES_IN_FLIGHT];
		VkSampler m_VkSampler[FRAMES_IN_FLIGHT];
		VmaAllocation m_VmaAllocation[FRAMES_IN_FLIGHT];

		// Staging buffer
		VkBuffer m_VkStagingBuffer[FRAMES_IN_FLIGHT];
		VmaAllocation m_VmaStagingAllocation[FRAMES_IN_FLIGHT];
		void* m_StagingMappedData[FRAMES_IN_FLIGHT];

		uint64_t m_Address = 0;

	public:
		friend class VulkanDescriptorSet;
	};
}
