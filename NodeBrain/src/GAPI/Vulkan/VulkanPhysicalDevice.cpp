#include "NBpch.h"
#include "VulkanPhysicalDevice.h"

namespace NodeBrain
{
	VulkanPhysicalDevice::VulkanPhysicalDevice(VkInstance instance, uint32_t deviceHandle, std::shared_ptr<VulkanSurface> surface)
		: m_VkInstance(instance), m_Surface(surface)
	{
		PickPhysicalDevice(deviceHandle);
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
		return indices.IsComplete();
	}

	QueueFamilyIndices VulkanPhysicalDevice::FindQueueFamilies(VkPhysicalDevice device)
	{
		QueueFamilyIndices indices;
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
}