#include "NBpch.h"
#include "VulkanPhysicalDevice.h"

#include <limits>

#include <glm/glm.hpp>

#include "Core/App.h"

namespace NodeBrain
{
	VulkanPhysicalDevice::VulkanPhysicalDevice(VkInstance instance, uint32_t deviceHandle, std::shared_ptr<VulkanSurface> surface)
		: m_VkInstance(instance), m_Surface(surface)
	{
		PickPhysicalDevice(deviceHandle);

		m_DeviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
	}

	void VulkanPhysicalDevice::PickPhysicalDevice(uint32_t deviceNumber)
	{
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(m_VkInstance, &deviceCount, nullptr);
		NB_ASSERT(deviceCount, "Could not find any GPUs with Vulkan support");
		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(m_VkInstance, &deviceCount, &devices[0]);

		for (const auto& device : devices)
		{
			// Use first suitable device for now. 
			if (IsDeviceSuitable(device))
			{
				m_PhysicalDevice = device;
				break;
			}
		}
		NB_ASSERT(m_PhysicalDevice, "Could not find suitable GPU");

	}

	bool VulkanPhysicalDevice::IsDeviceSuitable(VkPhysicalDevice device)
	{
		if (!device)
			return false;
		QueueFamilyIndices indices = FindQueueFamilies(device);
		bool extensionSupported = CheckDeviceExtensionSupport(device);

		bool swapChainAdequate = false;
		if (extensionSupported)
		{
			SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device);
			swapChainAdequate = !swapChainSupport.Formats.empty() && !swapChainSupport.PresentationModes.empty();
		}
		return indices.IsComplete() && extensionSupported && swapChainAdequate;
	}

	QueueFamilyIndices VulkanPhysicalDevice::FindQueueFamilies(VkPhysicalDevice device)
	{
		QueueFamilyIndices indices = {};
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, &queueFamilies[0]);

		for (int i = 0; i < queueFamilyCount; i++)
		{
			if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				VkBool32 presentationSupport = false;
				vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_Surface->GetVkSurface(), &presentationSupport);
				if (presentationSupport)
					indices.Presentation = i;

				indices.Graphics = i;
			}
		}

		return indices;
	}

	SwapChainSupportDetails VulkanPhysicalDevice::QuerySwapChainSupport(VkPhysicalDevice device)
	{
		// Capabilities
		SwapChainSupportDetails supportDetails = {};
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_Surface->GetVkSurface(), &supportDetails.Capabilities);

		// Formats
		uint32_t formatCount = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface->GetVkSurface(), &formatCount, nullptr);
		if (formatCount)
		{
			supportDetails.Formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface->GetVkSurface(), &formatCount, supportDetails.Formats.data());
		}

		// Presentation Modes
		uint32_t presentationModeCount = 0;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_Surface->GetVkSurface(), &presentationModeCount, nullptr);
		if (presentationModeCount)
		{
			supportDetails.PresentationModes.resize(presentationModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_Surface->GetVkSurface(), &presentationModeCount, supportDetails.PresentationModes.data());
		}


		return supportDetails;
	}

	bool VulkanPhysicalDevice::CheckDeviceExtensionSupport(VkPhysicalDevice device)
	{
		uint32_t extensionCount = 0;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, &availableExtensions[0]);

		std::set<std::string> requiredExtensions(m_DeviceExtensions.begin(), m_DeviceExtensions.end());

		for (const auto& extension : availableExtensions)
			requiredExtensions.erase(extension.extensionName);

		return requiredExtensions.empty();
	}

	VkSurfaceFormatKHR VulkanPhysicalDevice::ChooseSwapChainFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
	{
		for (const auto& format : availableFormats)
		{
			if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR)
				return format;
		}

		return availableFormats[0];
	}

	VkPresentModeKHR VulkanPhysicalDevice::ChooseSwapChainPresentationMode(const std::vector<VkPresentModeKHR>& availablePresentationModes)
	{
		for (const auto& presentationMode : availablePresentationModes)
		{
			if (presentationMode == VK_PRESENT_MODE_MAILBOX_KHR)
				return presentationMode;
		}
		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D VulkanPhysicalDevice::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
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