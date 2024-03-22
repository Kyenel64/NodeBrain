#pragma once

#include <vulkan/vulkan.h>

#include "GAPI/Vulkan/VulkanPhysicalDevice.h"
#include "GAPI/Vulkan/VulkanDevice.h"
#include "GAPI/Vulkan/VulkanImage.h"
#include "GAPI/Vulkan/VulkanFramebuffer.h"
#include "GAPI/Vulkan/VulkanRenderPass.h"

namespace NodeBrain
{
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
		VkFence GetInFlightFence() const { return m_InFlightFence; }
		VkSemaphore GetImageAvailableSemaphore() const { return m_ImageAvailableSemaphore; }
		VkSemaphore GetRenderFinishedSemaphore() const { return m_RenderFinishedSemaphore; }
		std::shared_ptr<VulkanRenderPass> GetRenderPass() const { return m_RenderPass; }
		std::shared_ptr<VulkanFramebuffer> GetCurrentFramebuffer() const { return m_Framebuffers[m_ImageIndex]; }
		uint32_t GetImageIndex() const { return m_ImageIndex; }
		
	private:
		void Init();

		VkSurfaceFormatKHR ChooseSwapChainFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR ChooseSwapChainPresentationMode(const std::vector<VkPresentModeKHR>& availablePresentationModes);
		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

	private:
		VkSwapchainKHR m_VkSwapChain = VK_NULL_HANDLE;
		VkSurfaceKHR m_VkSurface = VK_NULL_HANDLE;
		std::shared_ptr<VulkanDevice> m_Device;

		std::shared_ptr<VulkanRenderPass> m_RenderPass;

		std::vector<std::shared_ptr<VulkanImage>> m_SwapChainImages;
		
		std::vector<std::shared_ptr<VulkanFramebuffer>> m_Framebuffers;

		uint32_t m_ImageIndex = 0;

		// Configuration
		VkFormat m_ColorFormat;
		VkColorSpaceKHR m_ColorSpace;
		VkExtent2D m_Extent;
		VkPresentModeKHR m_PresentationMode;

		// Synchronizations
		VkSemaphore m_ImageAvailableSemaphore = VK_NULL_HANDLE;
		VkSemaphore m_RenderFinishedSemaphore = VK_NULL_HANDLE;
		VkFence m_InFlightFence = VK_NULL_HANDLE;

	};
}