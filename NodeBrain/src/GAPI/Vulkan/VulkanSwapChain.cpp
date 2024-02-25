#include "NBpch.h"
#include "VulkanSwapChain.h"

#include "Core/App.h"
#include "GAPI/Vulkan/VulkanRenderContext.h"

namespace NodeBrain
{
	VulkanSwapChain::VulkanSwapChain(VkSurfaceKHR surface, std::shared_ptr<VulkanDevice> device)
		: m_VkSurface(surface), m_Device(device)
	{
		NB_PROFILE_FN();

		Init();
	}

	VulkanSwapChain::~VulkanSwapChain()
	{
		if (m_VkSwapChain)
			Destroy();
	}

	void VulkanSwapChain::Destroy()
	{
		NB_PROFILE_FN();

		vkDestroySwapchainKHR(m_Device->GetVkDevice(), m_VkSwapChain, nullptr);
		m_VkSwapChain = VK_NULL_HANDLE;
	}

	void VulkanSwapChain::Init()
	{
		NB_PROFILE_FN();

		// Set configurations
		const SwapChainSupportDetails& swapChainSupport = m_Device->GetPhysicalDevice()->GetSwapChainSupportDetails();
		VkSurfaceFormatKHR surfaceFormat = ChooseSwapChainFormat(swapChainSupport.Formats);
		m_ColorFormat = surfaceFormat.format;
		m_ColorSpace = surfaceFormat.colorSpace;
		m_PresentationMode = ChooseSwapChainPresentationMode(swapChainSupport.PresentationModes);
		m_Extent = ChooseSwapExtent(swapChainSupport.Capabilities);

		uint32_t imageCount = swapChainSupport.Capabilities.minImageCount + 1;
		if (swapChainSupport.Capabilities.maxImageCount > 0 && imageCount > swapChainSupport.Capabilities.maxImageCount)
			imageCount = swapChainSupport.Capabilities.maxImageCount;

		// --- Create info ---
		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = m_VkSurface;
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = m_ColorFormat;
		createInfo.imageColorSpace = m_ColorSpace;
		createInfo.imageExtent = m_Extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		QueueFamilyIndices indices = m_Device->GetPhysicalDevice()->GetQueueFamilyIndices();
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
		createInfo.presentMode = m_PresentationMode;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = VK_NULL_HANDLE;

		VkResult result = vkCreateSwapchainKHR(m_Device->GetVkDevice(), &createInfo, nullptr, &m_VkSwapChain);
		NB_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan swap chain");

		std::vector<VkImage> swapchainImages;
		vkGetSwapchainImagesKHR(m_Device->GetVkDevice(), m_VkSwapChain, &imageCount, nullptr);
		swapchainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(m_Device->GetVkDevice(), m_VkSwapChain, &imageCount, &swapchainImages[0]);

		for (int i = 0; i < imageCount; i++)
			m_SwapChainImages.push_back(std::make_shared<VulkanImage>(m_Device, swapchainImages[i], m_ColorFormat));
	}

	VkSurfaceFormatKHR VulkanSwapChain::ChooseSwapChainFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
	{
		NB_PROFILE_FN();

		for (const auto& format : availableFormats)
		{
			if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR)
				return format;
		}

		return availableFormats[0];
	}

	VkPresentModeKHR VulkanSwapChain::ChooseSwapChainPresentationMode(const std::vector<VkPresentModeKHR>& availablePresentationModes)
	{
		NB_PROFILE_FN();

		for (const auto& presentationMode : availablePresentationModes)
		{
			if (presentationMode == VK_PRESENT_MODE_MAILBOX_KHR)
				return presentationMode;
		}
		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D VulkanSwapChain::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
	{
		NB_PROFILE_FN();

		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		{
			return capabilities.currentExtent;
		}
		else
		{
			glm::vec2 framebufferSize = App::GetInstance()->GetWindow().GetFramebufferSize();
			VkExtent2D actualExtent = { static_cast<uint32_t>(framebufferSize.x), static_cast<uint32_t>(framebufferSize.y) };
			actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
			actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

			return actualExtent;
		}
	}
}