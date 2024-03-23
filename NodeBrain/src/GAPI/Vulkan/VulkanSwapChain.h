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

	class VulkanSwapChain
	{
	public:
		VulkanSwapChain(VkSurfaceKHR surface, std::shared_ptr<VulkanDevice> device);
		~VulkanSwapChain();

		uint32_t AcquireNextImage();
		void PresentImage();

		VkSwapchainKHR GetVkSwapchain() const { return m_VkSwapChain; }
		VkFormat GetFormat() const { return m_ColorFormat; }
		VkExtent2D GetVkExtent() const { return m_Extent; }
		std::shared_ptr<VulkanRenderPass> GetRenderPass() const { return m_RenderPass; }
		std::shared_ptr<VulkanFramebuffer> GetCurrentFramebuffer() const { return m_Framebuffers[m_ImageIndex]; }
		const FrameData& GetCurrentFrameData() const { return m_FrameDatas[m_CurrentFrame]; }
		uint32_t GetImageIndex() const { return m_ImageIndex; }
		
	private:
		void Init();

	private:
		VkSwapchainKHR m_VkSwapChain = VK_NULL_HANDLE;
		VkSurfaceKHR m_VkSurface = VK_NULL_HANDLE;
		std::shared_ptr<VulkanDevice> m_Device;

		// Configuration
		VkFormat m_ColorFormat;
		VkColorSpaceKHR m_ColorSpace;
		VkExtent2D m_Extent;
		VkPresentModeKHR m_PresentationMode;

		uint32_t m_ImageIndex = 0;
		uint32_t m_CurrentFrame = 0;

		std::shared_ptr<VulkanRenderPass> m_RenderPass;
		std::vector<std::shared_ptr<VulkanImage>> m_SwapChainImages;
		std::vector<std::shared_ptr<VulkanFramebuffer>> m_Framebuffers;

		FrameData m_FrameDatas[FRAMES_IN_FLIGHT];

	};
}