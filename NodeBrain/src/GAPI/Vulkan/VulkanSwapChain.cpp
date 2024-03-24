#include "NBpch.h"
#include "VulkanSwapChain.h"

#include "Core/App.h"

namespace NodeBrain
{
	namespace Utils
	{
		static VkSurfaceFormatKHR ChooseSwapChainFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
		{
			for (const auto& format : availableFormats)
			{
				if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR)
					return format;
			}

			return availableFormats[0];
		}

		static VkPresentModeKHR ChooseSwapChainPresentationMode(const std::vector<VkPresentModeKHR>& availablePresentationModes)
		{
			for (const auto& presentationMode : availablePresentationModes)
			{
				if (presentationMode == VK_PRESENT_MODE_MAILBOX_KHR)
					return presentationMode;
			}
			return VK_PRESENT_MODE_FIFO_KHR;
		}

		static VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
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
	}




	VulkanSwapChain::VulkanSwapChain(VkSurfaceKHR surface, std::shared_ptr<VulkanDevice> device)
		: m_VkSurface(surface), m_Device(device), m_ImageIndex(0)
	{
		NB_PROFILE_FN();

		Init();
	}

	VulkanSwapChain::~VulkanSwapChain()
	{
		NB_PROFILE_FN();
		
		for (size_t i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			vkDestroyCommandPool(m_Device->GetVkDevice(), m_FrameDatas[i].CommandPool, nullptr);
			m_FrameDatas[i].CommandPool = VK_NULL_HANDLE;
			vkDestroySemaphore(m_Device->GetVkDevice(), m_FrameDatas[i].RenderFinishedSemaphore, nullptr);
			m_FrameDatas[i].RenderFinishedSemaphore = VK_NULL_HANDLE;
			vkDestroySemaphore(m_Device->GetVkDevice(), m_FrameDatas[i].ImageAvailableSemaphore, nullptr);
			m_FrameDatas[i].ImageAvailableSemaphore = VK_NULL_HANDLE;
			vkDestroyFence(m_Device->GetVkDevice(), m_FrameDatas[i].InFlightFence, nullptr);
			m_FrameDatas[i].InFlightFence = VK_NULL_HANDLE;
		}

		if (!m_VkFramebuffers.empty())
		{
			for (auto framebuffer : m_VkFramebuffers)
				vkDestroyFramebuffer(m_Device->GetVkDevice(), framebuffer, nullptr);
		}

		if (!m_VkImageViews.empty())
		{
			for (auto imageView : m_VkImageViews)
				vkDestroyImageView(m_Device->GetVkDevice(), imageView, nullptr);
		}

		if (m_VkRenderPass)
		{
			vkDestroyRenderPass(m_Device->GetVkDevice(), m_VkRenderPass, nullptr);
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
		VkSurfaceFormatKHR surfaceFormat = Utils::ChooseSwapChainFormat(swapChainSupport.Formats);
		m_VkColorFormat = surfaceFormat.format;
		m_VkColorSpace = surfaceFormat.colorSpace;
		m_VkPresentationMode = Utils::ChooseSwapChainPresentationMode(swapChainSupport.PresentationModes);
		m_VkExtent = Utils::ChooseSwapExtent(swapChainSupport.Capabilities);

		uint32_t imageCount = swapChainSupport.Capabilities.minImageCount + 1;
		if (swapChainSupport.Capabilities.maxImageCount > 0 && imageCount > swapChainSupport.Capabilities.maxImageCount)
			imageCount = swapChainSupport.Capabilities.maxImageCount;

		// --- Create swapchain ---
		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = m_VkSurface;
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = m_VkColorFormat;
		createInfo.imageColorSpace = m_VkColorSpace;
		createInfo.imageExtent = m_VkExtent;
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
		createInfo.presentMode = m_VkPresentationMode;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = VK_NULL_HANDLE;

		VK_CHECK(vkCreateSwapchainKHR(m_Device->GetVkDevice(), &createInfo, nullptr, &m_VkSwapChain));


		// --- Create image & views ---
		uint32_t vkImageCount = 0;
		vkGetSwapchainImagesKHR(m_Device->GetVkDevice(), m_VkSwapChain, &vkImageCount, nullptr);
		m_VkImages.resize(vkImageCount);
		m_VkImageViews.resize(vkImageCount);
		m_VkFramebuffers.resize(vkImageCount);
		vkGetSwapchainImagesKHR(m_Device->GetVkDevice(), m_VkSwapChain, &vkImageCount, &m_VkImages[0]);

		for (size_t i = 0; i < vkImageCount; i++)
		{
			VkImageViewCreateInfo imageViewCreateInfo = {};
			imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			imageViewCreateInfo.image = m_VkImages[i];
			imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D; // TODO: parameterize
			imageViewCreateInfo.format = m_VkColorFormat;

			// TODO: parameterize
			imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
			imageViewCreateInfo.subresourceRange.levelCount = 1;
			imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
			imageViewCreateInfo.subresourceRange.layerCount = 1;

			VK_CHECK(vkCreateImageView(m_Device->GetVkDevice(), &imageViewCreateInfo, nullptr, &m_VkImageViews[i]));
		}


		// --- Create render pass ---
		VkAttachmentDescription colorAttachment = {};
		colorAttachment.format = VK_FORMAT_B8G8R8A8_SRGB; // temp. parametrize
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		// Subpass
		VkAttachmentReference colorAttachmentRef = {};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;

		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		VkRenderPassCreateInfo renderPassCreateInfo = {};
		renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassCreateInfo.attachmentCount = 1;
		renderPassCreateInfo.pAttachments = &colorAttachment;
		renderPassCreateInfo.subpassCount = 1;
		renderPassCreateInfo.pSubpasses = &subpass;
		renderPassCreateInfo.dependencyCount = 1;
		renderPassCreateInfo.pDependencies = &dependency;

		VK_CHECK(vkCreateRenderPass(m_Device->GetVkDevice(), &renderPassCreateInfo, nullptr, &m_VkRenderPass));


		// --- Create framebuffers ---
		for (int i = 0; i < vkImageCount; i++)
		{
			VkImageView attachments[] = { m_VkImageViews[i] };

			VkFramebufferCreateInfo framebufferCreateInfo = {};
			framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferCreateInfo.renderPass = m_VkRenderPass;
			framebufferCreateInfo.attachmentCount = 1;
			framebufferCreateInfo.pAttachments = attachments;
			framebufferCreateInfo.width = m_VkExtent.width;
			framebufferCreateInfo.height = m_VkExtent.height;
			framebufferCreateInfo.layers = 1;

			VK_CHECK(vkCreateFramebuffer(m_Device->GetVkDevice(), &framebufferCreateInfo, nullptr, &m_VkFramebuffers[i]));
		}


		// --- Create synchronization objects ---
		for (size_t i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			QueueFamilyIndices queueFamilyIndices = m_Device->GetPhysicalDevice()->GetQueueFamilyIndices();
			VkCommandPoolCreateInfo commandPoolCreateInfo = {};
			commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			commandPoolCreateInfo.queueFamilyIndex = queueFamilyIndices.Graphics.value();
			VK_CHECK(vkCreateCommandPool(m_Device->GetVkDevice(), &commandPoolCreateInfo, nullptr, &m_FrameDatas[i].CommandPool));

			VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
			commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			commandBufferAllocateInfo.commandPool = m_FrameDatas[i].CommandPool;
			commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			commandBufferAllocateInfo.commandBufferCount = 1;
			VK_CHECK(vkAllocateCommandBuffers(m_Device->GetVkDevice(), &commandBufferAllocateInfo, &m_FrameDatas[i].CommandBuffer));

			VkSemaphoreCreateInfo semaphoreCreateInfo = {};
			semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
			VK_CHECK(vkCreateSemaphore(m_Device->GetVkDevice(), &semaphoreCreateInfo, nullptr, &m_FrameDatas[i].ImageAvailableSemaphore));
			VK_CHECK(vkCreateSemaphore(m_Device->GetVkDevice(), &semaphoreCreateInfo, nullptr, &m_FrameDatas[i].RenderFinishedSemaphore));

			VkFenceCreateInfo fenceCreateInfo = {};
			fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
			VK_CHECK(vkCreateFence(m_Device->GetVkDevice(), &fenceCreateInfo, nullptr, &m_FrameDatas[i].InFlightFence));
		}
	}

	uint32_t VulkanSwapChain::AcquireNextImage()
	{
		vkWaitForFences(m_Device->GetVkDevice(), 1, &m_FrameDatas[m_CurrentFrame].InFlightFence, VK_TRUE, UINT64_MAX);

		VkResult result = vkAcquireNextImageKHR(m_Device->GetVkDevice(), m_VkSwapChain, UINT64_MAX, m_FrameDatas[m_CurrentFrame].ImageAvailableSemaphore, VK_NULL_HANDLE, &m_ImageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
			NB_INFO("RecreateSwapchain"); //RecreateSwapchain();

		vkResetFences(m_Device->GetVkDevice(), 1, &m_FrameDatas[m_CurrentFrame].InFlightFence);
		//m_RenderPass->SetTargetFramebuffer(m_Framebuffers[m_ImageIndex]);
		return m_ImageIndex;
	}

	void VulkanSwapChain::PresentImage()
	{
		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &m_FrameDatas[m_CurrentFrame].RenderFinishedSemaphore;

		VkSwapchainKHR swapChains[] = { m_VkSwapChain };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &m_ImageIndex;
		presentInfo.pResults = nullptr; // Optional

		vkQueuePresentKHR(m_Device->GetPresentationQueue(), &presentInfo);

		m_CurrentFrame = (m_CurrentFrame + 1) % FRAMES_IN_FLIGHT;
	}
}