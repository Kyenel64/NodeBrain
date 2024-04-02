#include "NBpch.h"
#include "VulkanPhysicalDevice.h"

namespace NodeBrain
{
	VulkanPhysicalDevice::VulkanPhysicalDevice(VkInstance instance, VkSurfaceKHR surface, uint32_t deviceIndex)
		: m_VkInstance(instance), m_VkSurfaceKHR(surface), m_DeviceIndex(deviceIndex)
	{
		NB_PROFILE_FN();

		m_EnabledDeviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
		m_EnabledDeviceExtensions.push_back(VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME);
		#if NB_APPLE
			m_EnabledDeviceExtensions.push_back("VK_KHR_portability_subset");
		#endif

		// Retrieve the VkPhysicalDevice at m_DeviceIndex
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(m_VkInstance, &deviceCount, nullptr);
		NB_ASSERT(deviceCount, "Could not find any GPUs with Vulkan support");
		NB_ASSERT(m_DeviceIndex < deviceCount, "Physical device index out of bounds");
		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(m_VkInstance, &deviceCount, &devices[0]);
		m_VkPhysicalDevice = devices[m_DeviceIndex];
	}

	bool VulkanPhysicalDevice::IsSuitable()
	{
		NB_PROFILE_FN();

		if (!m_VkPhysicalDevice)
			return false;

		bool extensionSupported = CheckDeviceExtensionSupport();
		SwapchainSupportDetails swapchainSupportDetails = QuerySwapchainSupport();
		QueueFamilyIndices queueFamilyIndices = FindQueueFamilies();

		bool swapchainAdequate = false;
		if (extensionSupported)
			swapchainAdequate = !swapchainSupportDetails.Formats.empty() && !swapchainSupportDetails.PresentationModes.empty();

		return queueFamilyIndices.IsComplete() && extensionSupported && swapchainAdequate;
	}

	QueueFamilyIndices VulkanPhysicalDevice::FindQueueFamilies() const
	{
		NB_PROFILE_FN();

		QueueFamilyIndices indices = {};
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(m_VkPhysicalDevice, &queueFamilyCount, nullptr);
		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(m_VkPhysicalDevice, &queueFamilyCount, &queueFamilies[0]);

		for (int i = 0; i < queueFamilyCount; i++)
		{
			if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				VkBool32 presentationSupport = false;
				vkGetPhysicalDeviceSurfaceSupportKHR(m_VkPhysicalDevice, i, m_VkSurfaceKHR, &presentationSupport);
				if (presentationSupport)
					indices.Presentation = i;

				indices.Graphics = i;
			}
		}

		return indices;
	}

	SwapchainSupportDetails VulkanPhysicalDevice::QuerySwapchainSupport() const
	{
		NB_PROFILE_FN();

		// Capabilities
		SwapchainSupportDetails supportDetails = {};
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_VkPhysicalDevice, m_VkSurfaceKHR, &supportDetails.Capabilities);

		// Formats
		uint32_t formatCount = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR(m_VkPhysicalDevice, m_VkSurfaceKHR, &formatCount, nullptr);
		if (formatCount)
		{
			supportDetails.Formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(m_VkPhysicalDevice, m_VkSurfaceKHR, &formatCount, supportDetails.Formats.data());
		}

		// Presentation Modes
		uint32_t presentationModeCount = 0;
		vkGetPhysicalDeviceSurfacePresentModesKHR(m_VkPhysicalDevice, m_VkSurfaceKHR, &presentationModeCount, nullptr);
		if (presentationModeCount)
		{
			supportDetails.PresentationModes.resize(presentationModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(m_VkPhysicalDevice, m_VkSurfaceKHR, &presentationModeCount, supportDetails.PresentationModes.data());
		}


		return supportDetails;
	}

	bool VulkanPhysicalDevice::CheckDeviceExtensionSupport() const
	{
		NB_PROFILE_FN();

		uint32_t extensionCount = 0;
		vkEnumerateDeviceExtensionProperties(m_VkPhysicalDevice, nullptr, &extensionCount, nullptr);
		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(m_VkPhysicalDevice, nullptr, &extensionCount, &availableExtensions[0]);

		std::set<std::string> requiredExtensions(m_EnabledDeviceExtensions.begin(), m_EnabledDeviceExtensions.end());

		for (const auto& extension : availableExtensions)
			requiredExtensions.erase(extension.extensionName);

		return requiredExtensions.empty();
	}
}