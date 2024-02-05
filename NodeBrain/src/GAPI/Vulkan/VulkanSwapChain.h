#pragma once

#include <vulkan/vulkan.h>

#include "GAPI/Vulkan/VulkanPhysicalDevice.h"
#include "GAPI/Vulkan/VulkanDevice.h"
#include "GAPI/Vulkan/VulkanSurface.h"

namespace NodeBrain
{
	class VulkanSwapChain
	{
	public:
		VulkanSwapChain(std::shared_ptr<VulkanDevice> device);
		~VulkanSwapChain();

	private:
		void Init();

	private:
		VkSwapchainKHR m_VkSwapChain = VK_NULL_HANDLE;
		std::shared_ptr<VulkanPhysicalDevice> m_PhysicalDevice;
		std::shared_ptr<VulkanDevice> m_Device;
		std::shared_ptr<VulkanSurface> m_Surface;

		std::vector<VkImage> m_SwapChainImages;
		VkFormat m_ImageFormat;
		VkExtent2D m_Extent;
	};
}