#pragma once

#include <vulkan/vulkan.h>

#include "GAPI/Vulkan/VulkanPhysicalDevice.h"
#include "GAPI/Vulkan/VulkanDevice.h"
#include "GAPI/Vulkan/VulkanImage.h"

namespace NodeBrain
{
	class VulkanSwapChain
	{
	public:
		VulkanSwapChain(VkSurfaceKHR surface, std::shared_ptr<VulkanDevice> device);
		~VulkanSwapChain();

		VkFormat GetFormat() const { return m_ColorFormat; }

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