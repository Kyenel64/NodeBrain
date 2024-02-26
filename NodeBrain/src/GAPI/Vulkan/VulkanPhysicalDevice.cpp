#include "NBpch.h"
#include "VulkanPhysicalDevice.h"

#include <limits>

#include <glm/glm.hpp>

#include "Core/App.h"
#include "GAPI/Vulkan/VulkanRenderContext.h"

namespace NodeBrain
{
	VulkanPhysicalDevice::VulkanPhysicalDevice(VkInstance instance, uint32_t deviceIndex, VkSurfaceKHR surface)
		: m_VkInstance(instance), m_DeviceIndex(deviceIndex), m_VkSurfaceKHR(surface)
	{
		NB_PROFILE_FN();

		m_DeviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
		#if NB_APPLE
			m_DeviceExtensions.push_back("VK_KHR_portability_subset");
		#endif

		Init();
	}

	void VulkanPhysicalDevice::Init()
	{
		// Set VkPhysicalDevice from device index
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(m_VkInstance, &deviceCount, nullptr);
		NB_ASSERT(deviceCount, "Could not find any GPUs with Vulkan support");
		NB_ASSERT(m_DeviceIndex < deviceCount, "Physical device index out of bounds");
		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(m_VkInstance, &deviceCount, &devices[0]);
		m_VkPhysicalDevice = devices[m_DeviceIndex];

		m_QueueFamilyIndices = FindQueueFamilies();
		m_SwapChainSupportDetails = QuerySwapChainSupport();
	}

	bool VulkanPhysicalDevice::IsSuitable()
	{
		NB_PROFILE_FN();

		if (!m_VkPhysicalDevice)
			return false;

		bool extensionSupported = CheckDeviceExtensionSupport();

		bool swapChainAdequate = false;
		if (extensionSupported)
			swapChainAdequate = !m_SwapChainSupportDetails.Formats.empty() && !m_SwapChainSupportDetails.PresentationModes.empty();

		return m_QueueFamilyIndices.IsComplete() && extensionSupported && swapChainAdequate;
	}

	QueueFamilyIndices VulkanPhysicalDevice::FindQueueFamilies()
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

	SwapChainSupportDetails VulkanPhysicalDevice::QuerySwapChainSupport()
	{
		NB_PROFILE_FN();

		// Capabilities
		SwapChainSupportDetails supportDetails = {};
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

	bool VulkanPhysicalDevice::CheckDeviceExtensionSupport()
	{
		NB_PROFILE_FN();

		uint32_t extensionCount = 0;
		vkEnumerateDeviceExtensionProperties(m_VkPhysicalDevice, nullptr, &extensionCount, nullptr);
		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(m_VkPhysicalDevice, nullptr, &extensionCount, &availableExtensions[0]);

		std::set<std::string> requiredExtensions(m_DeviceExtensions.begin(), m_DeviceExtensions.end());

		for (const auto& extension : availableExtensions)
			requiredExtensions.erase(extension.extensionName);

		return requiredExtensions.empty();
	}

	
}