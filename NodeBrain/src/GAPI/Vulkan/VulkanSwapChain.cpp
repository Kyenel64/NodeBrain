#include "NBpch.h"
#include "VulkanSwapChain.h"

#include "Core/App.h"

namespace NodeBrain
{
	VulkanSwapChain::VulkanSwapChain(VkSurfaceKHR surface, std::shared_ptr<VulkanDevice> device)
		: m_VkSurface(surface), m_Device(device), m_ImageIndex(0)
	{
		NB_PROFILE_FN();

		Init();
	}

	VulkanSwapChain::~VulkanSwapChain()
	{
		NB_PROFILE_FN();
		
		if (m_ImageAvailableSemaphore)
		{
			vkDestroySemaphore(m_Device->GetVkDevice(), m_ImageAvailableSemaphore, nullptr);
			m_ImageAvailableSemaphore = VK_NULL_HANDLE;
		}
		if (m_RenderFinishedSemaphore)
		{
			vkDestroySemaphore(m_Device->GetVkDevice(), m_RenderFinishedSemaphore, nullptr);
			m_RenderFinishedSemaphore = VK_NULL_HANDLE;
		}
		if (m_InFlightFence)
		{
			vkDestroyFence(m_Device->GetVkDevice(), m_InFlightFence, nullptr);
			m_InFlightFence = VK_NULL_HANDLE;
		}

		if (!m_Framebuffers.empty())
		{
			m_Framebuffers.clear();
		}

		// current framebuffer doesnt destroy if owned by renderpass for some reason
		m_RenderPass->SetTargetFramebuffer(nullptr);

		if (m_RenderPass)
		{
			m_RenderPass.reset();
		}

		if (!m_SwapChainImages.empty())
		{
			m_SwapChainImages.clear();
		}

		if (m_VkSwapChain)
		{
			vkDestroySwapchainKHR(m_Device->GetVkDevice(), m_VkSwapChain, nullptr);
			m_VkSwapChain = VK_NULL_HANDLE;
		}
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

		// --- Create swapchain ---
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

		// --- Create image & views ---
		std::vector<VkImage> images;
		vkGetSwapchainImagesKHR(m_Device->GetVkDevice(), m_VkSwapChain, &imageCount, nullptr);
		images.resize(imageCount);
		vkGetSwapchainImagesKHR(m_Device->GetVkDevice(), m_VkSwapChain, &imageCount, &images[0]);
		for (int i = 0; i < imageCount; i++)
			m_SwapChainImages.push_back(std::make_shared<VulkanImage>(m_Device, images[i], m_ColorFormat));

		// --- Create render pass ---
		m_RenderPass = std::make_shared<VulkanRenderPass>();

		// --- Create framebuffers ---
		for (int i = 0; i < imageCount; i++)
		{
			FramebufferConfiguration config = {};
			config.Width = m_Extent.width;
			config.Height = m_Extent.height;
			config.Image = m_SwapChainImages[i];
			config.RenderPass = m_RenderPass;
			m_Framebuffers.push_back(std::make_shared<VulkanFramebuffer>(config));
		}
		m_RenderPass->SetTargetFramebuffer(m_Framebuffers[m_ImageIndex]);

		// --- Create synchronization objects ---
		VkSemaphoreCreateInfo semaphoreCreateInfo = {};
		semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceCreateInfo = {};
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		vkCreateSemaphore(m_Device->GetVkDevice(), &semaphoreCreateInfo, nullptr, &m_ImageAvailableSemaphore);
		vkCreateSemaphore(m_Device->GetVkDevice(), &semaphoreCreateInfo, nullptr, &m_RenderFinishedSemaphore);
		vkCreateFence(m_Device->GetVkDevice(), &fenceCreateInfo, nullptr, &m_InFlightFence);
	}

	VkSurfaceFormatKHR VulkanSwapChain::ChooseSwapChainFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
	{
		for (const auto& format : availableFormats)
		{
			if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR)
				return format;
		}

		return availableFormats[0];
	}

	VkPresentModeKHR VulkanSwapChain::ChooseSwapChainPresentationMode(const std::vector<VkPresentModeKHR>& availablePresentationModes)
	{
		for (const auto& presentationMode : availablePresentationModes)
		{
			if (presentationMode == VK_PRESENT_MODE_MAILBOX_KHR)
				return presentationMode;
		}
		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D VulkanSwapChain::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
	{
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

	uint32_t VulkanSwapChain::AcquireNextImage()
	{
		vkWaitForFences(m_Device->GetVkDevice(), 1, &m_InFlightFence, VK_TRUE, UINT64_MAX);
		vkResetFences(m_Device->GetVkDevice(), 1, &m_InFlightFence);

		vkAcquireNextImageKHR(m_Device->GetVkDevice(), m_VkSwapChain, UINT64_MAX, m_ImageAvailableSemaphore, VK_NULL_HANDLE, &m_ImageIndex);
		m_RenderPass->SetTargetFramebuffer(m_Framebuffers[m_ImageIndex]);
		return m_ImageIndex;
	}

	void VulkanSwapChain::PresentImage()
	{
		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &m_RenderFinishedSemaphore;

		VkSwapchainKHR swapChains[] = { m_VkSwapChain };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &m_ImageIndex;
		presentInfo.pResults = nullptr; // Optional

		vkQueuePresentKHR(m_Device->GetPresentationQueue(), &presentInfo);
	}
}