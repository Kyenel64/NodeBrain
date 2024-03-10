#pragma once

#include <vulkan/vulkan.h>

#include "GAPI/Vulkan/VulkanPhysicalDevice.h"
#include "GAPI/Vulkan/VulkanDevice.h"
#include "GAPI/Vulkan/VulkanImage.h"
#include "GAPI/Vulkan/VulkanFramebuffer.h"

namespace NodeBrain
{
	class VulkanSwapChain
	{
	public:
		VulkanSwapChain(VkSurfaceKHR surface, std::shared_ptr<VulkanDevice> device);
		~VulkanSwapChain();

		VkSwapchainKHR GetVkSwapchain() const { return m_VkSwapChain; }
		VkFormat GetFormat() const { return m_ColorFormat; }
		const std::vector<std::shared_ptr<VulkanImage>>& GetSwapchainImages() const { return m_SwapChainImages; }

		VkExtent2D GetVkExtent() const { return m_Extent; }
		float GetExtentWidth() const { return m_Extent.width; }
		float GetExtentHeight() const { return m_Extent.height; }

	private:
		void Init();

		VkSurfaceFormatKHR ChooseSwapChainFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR ChooseSwapChainPresentationMode(const std::vector<VkPresentModeKHR>& availablePresentationModes);
		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

	private:
		VkSwapchainKHR m_VkSwapChain = VK_NULL_HANDLE;
		VkSurfaceKHR m_VkSurface = VK_NULL_HANDLE;
		std::shared_ptr<VulkanDevice> m_Device;

		std::vector<std::shared_ptr<VulkanImage>> m_SwapChainImages;

		// Configuration
		VkFormat m_ColorFormat;
		VkColorSpaceKHR m_ColorSpace;
		VkExtent2D m_Extent;
		VkPresentModeKHR m_PresentationMode;
	};
}