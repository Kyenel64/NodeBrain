#pragma once

#include "Renderer/Framebuffer.h"
#include "GAPI/Vulkan/VulkanRenderContext.h"

namespace NodeBrain
{
	class VulkanFramebuffer : public Framebuffer
	{
	public:
		VulkanFramebuffer(VulkanRenderContext& context, const FramebufferConfiguration& configuration);
		~VulkanFramebuffer() override;

		void Resize(uint32_t width, uint32_t height) override;

		[[nodiscard]] uint64_t GetAddress() override;
		[[nodiscard]] const FramebufferConfiguration& GetConfiguration() const override { return m_Configuration; }

		[[nodiscard]] VkImage GetVkImage() const { return m_VkImage[m_Context.GetSwapchain().GetFrameIndex()]; }
		[[nodiscard]] VkImageView GetVkImageView() const { return m_VkImageView[m_Context.GetSwapchain().GetFrameIndex()]; }
		[[nodiscard]] VkSampler GetVkSampler() const { return m_VkSampler[m_Context.GetSwapchain().GetFrameIndex()]; }

	private:
		// Helper function which can be called during Resize()
		VkResult Create();
		// Helper function which can be called during Resize()
		void Destroy();

	private:
		VulkanRenderContext& m_Context;

		VkImage m_VkImage[FRAMES_IN_FLIGHT];
		VkImageView m_VkImageView[FRAMES_IN_FLIGHT];
		VkSampler m_VkSampler[FRAMES_IN_FLIGHT];
		VmaAllocation m_VmaAllocation[FRAMES_IN_FLIGHT];

		FramebufferConfiguration m_Configuration;

		uint64_t m_Address = 0;
	};
}