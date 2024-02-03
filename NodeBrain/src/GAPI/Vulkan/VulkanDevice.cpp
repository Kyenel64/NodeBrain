#include "NBpch.h"
#include "VulkanDevice.h"

namespace NodeBrain
{
	VulkanDevice::VulkanDevice(std::shared_ptr<VulkanPhysicalDevice> physicalDevice, std::shared_ptr<VulkanValidationLayer> validationLayer)
		: m_PhysicalDevice(physicalDevice), m_ValidationLayer(validationLayer)
	{
		if (validationLayer)
			m_EnableValidationLayers = true;

		Init();
	}

	VulkanDevice::~VulkanDevice()
	{
		vkDestroyDevice(m_Device, nullptr);
	}

	void VulkanDevice::Init()
	{
		// --- Queue info ---
		QueueFamilyIndices indices = m_PhysicalDevice->GetQueueFamilyIndices();
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = indices.Graphics.value();
		queueCreateInfo.queueCount = 1;
		float queuePriority = 1.0f;
		queueCreateInfo.pQueuePriorities = &queuePriority;

		// --- Features ---
		VkPhysicalDeviceFeatures deviceFeatures = {}; // TODO:

		// --- Device ---
		VkDeviceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.pQueueCreateInfos = &queueCreateInfo;
		createInfo.queueCreateInfoCount = 1;
		createInfo.pEnabledFeatures = &deviceFeatures;

		// Extensions
		createInfo.enabledExtensionCount = 0; // TODO:

		// Layers
		createInfo.enabledLayerCount = 0;
		if (m_EnableValidationLayers)
		{
			createInfo.enabledLayerCount = m_ValidationLayer->GetLayerCount();
			createInfo.ppEnabledLayerNames = m_ValidationLayer->GetLayers().data();
		}

		// Create device
		VkResult result = vkCreateDevice(m_PhysicalDevice->GetVkPhysicalDevice(), &createInfo, nullptr, &m_Device);
		NB_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan device");

		// Create graphics queue
		vkGetDeviceQueue(m_Device, indices.Graphics.value(), 0, &m_GraphicsQueue);
	}
}