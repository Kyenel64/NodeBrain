#include "NBpch.h"
#include "VulkanSwapChain.h"

#include "GAPI/Vulkan/VulkanRenderContext.h"

namespace NodeBrain
{
	VulkanSwapChain::VulkanSwapChain(std::shared_ptr<VulkanDevice> device)
		: m_Device(device)
	{
		m_PhysicalDevice = m_Device->GetPhysicalDevice();
		m_Surface = VulkanRenderContext::GetInstance()->GetSurface();
		Init();
	}

	VulkanSwapChain::~VulkanSwapChain()
	{
		vkDestroySwapchainKHR(m_Device->GetVkDevice(), m_VkSwapChain, nullptr);
	}

	void VulkanSwapChain::Init()
	{
		SwapChainSupportDetails swapChainSupport = m_PhysicalDevice->GetSwapChainSupportDetails();

		VkSurfaceFormatKHR surfaceFormat = VulkanPhysicalDevice::ChooseSwapChainFormat(swapChainSupport.Formats);
		VkPresentModeKHR presentationMode = VulkanPhysicalDevice::ChooseSwapChainPresentationMode(swapChainSupport.PresentationModes);
		VkExtent2D extent = VulkanPhysicalDevice::ChooseSwapExtent(swapChainSupport.Capabilities);

		uint32_t imageCount = swapChainSupport.Capabilities.minImageCount + 1;
		if (swapChainSupport.Capabilities.maxImageCount > 0 && imageCount > swapChainSupport.Capabilities.maxImageCount)
			imageCount = swapChainSupport.Capabilities.maxImageCount;

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = m_Surface->GetVkSurface();
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		QueueFamilyIndices indices = m_PhysicalDevice->GetQueueFamilyIndices();
		uint32_t queueFamilyIndices[] = { indices.Graphics.value(), indices.Presentation.value() };

		if (indices.Graphics != indices.Presentation) 
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else 
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0;
			createInfo.pQueueFamilyIndices = nullptr;
		}
		createInfo.preTransform = swapChainSupport.Capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = presentationMode;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = VK_NULL_HANDLE;

		VkResult result = vkCreateSwapchainKHR(m_Device->GetVkDevice(), &createInfo, nullptr, &m_VkSwapChain);
		NB_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan swap chain");

		m_ImageFormat = surfaceFormat.format;
		m_Extent = extent;
	}
}