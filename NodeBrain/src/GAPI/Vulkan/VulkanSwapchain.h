#pragma once

#include <vulkan/vulkan.h>

#include "GAPI/Vulkan/VulkanPhysicalDevice.h"
#include "GAPI/Vulkan/VulkanDevice.h"
#include "GAPI/Vulkan/VulkanImage.h"
#include "GAPI/Vulkan/VulkanFramebuffer.h"
#include "GAPI/Vulkan/VulkanRenderPass.h"

namespace NodeBrain
{
	#define FRAMES_IN_FLIGHT 2

	struct FrameData
	{
		VkCommandPool CommandPool;
		VkCommandBuffer CommandBuffer;
		VkSemaphore ImageAvailableSemaphore = VK_NULL_HANDLE;
		VkSemaphore RenderFinishedSemaphore = VK_NULL_HANDLE;
		VkFence InFlightFence = VK_NULL_HANDLE;
	};

	class VulkanSwapchain
	{
	public:
		VulkanSwapchain(VkSurfaceKHR surface, std::shared_ptr<VulkanDevice> device);
		~VulkanSwapchain();

		uint32_t AcquireNextImage();
		void PresentImage();
		void CopyImage(std::shared_ptr<VulkanImage> image);

		VkSwapchainKHR GetVkSwapchain() const { return m_VkSwapchain; }
		VkFormat GetVkFormat() const { return m_VkColorFormat; }
		VkExtent2D GetVkExtent() const { return m_VkExtent; }
		VkRenderPass GetVkRenderPass() const { return m_VkRenderPass; }
		VkFramebuffer GetCurrentVkFramebuffer() const { return m_VkFramebuffers[m_ImageIndex]; }
		const FrameData& GetCurrentFrameData() const { return m_FrameDatas[m_CurrentFrame]; }
		uint32_t GetImageIndex() const { return m_ImageIndex; }
		VkImage GetCurrentVkImage() const { return m_VkImages[m_ImageIndex]; }
		
	private:
		void Init();
		void RecreateSwapchain();

	private:
		VkSwapchainKHR m_VkSwapchain = VK_NULL_HANDLE;
		VkSurfaceKHR m_VkSurface = VK_NULL_HANDLE;
		std::shared_ptr<VulkanDevice> m_Device;

		// Configuration
		VkFormat m_VkColorFormat;
		VkColorSpaceKHR m_VkColorSpace;
		VkExtent2D m_VkExtent;
		VkPresentModeKHR m_VkPresentationMode;

		uint32_t m_ImageIndex = 0;
		uint32_t m_CurrentFrame = 0;

		VkRenderPass m_VkRenderPass;
		std::vector<VkImage> m_VkImages;
		std::vector<VkImageView> m_VkImageViews;
		std::vector<VkFramebuffer> m_VkFramebuffers;

		FrameData m_FrameDatas[FRAMES_IN_FLIGHT];

	};
}