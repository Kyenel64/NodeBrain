#include "NBpch.h"
#include "VulkanDevice.h"

#include "GAPI/Vulkan/VulkanRenderContext.h"

namespace NodeBrain
{
	VulkanDevice::VulkanDevice(std::shared_ptr<VulkanPhysicalDevice> physicalDevice)
		: m_PhysicalDevice(physicalDevice)
	{
		NB_PROFILE_FN();

		Init();
	}

	VulkanDevice::~VulkanDevice()
	{
		NB_PROFILE_FN();

		if (m_Device)
			Destroy();
	}

	void VulkanDevice::Destroy()
	{
		NB_PROFILE_FN();

		vkDestroyDevice(m_Device, nullptr);
		m_Device = VK_NULL_HANDLE;
	}

	void VulkanDevice::Init()
	{
		NB_PROFILE_FN();

		// --- Queue info ---
		QueueFamilyIndices indices = m_PhysicalDevice->GetQueueFamilyIndices();
		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = { indices.Graphics.value(), indices.Presentation.value() };
		float queuePriority = 1.0f;
		for (uint32_t queueFamily : uniqueQueueFamilies)
		{
			VkDeviceQueueCreateInfo queueCreateInfo = {};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}
		
		// --- Features ---
		VkPhysicalDeviceFeatures deviceFeatures = {}; // TODO:

		// --- Device ---
		VkDeviceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pQueueCreateInfos = &queueCreateInfos[0];
		createInfo.pEnabledFeatures = &deviceFeatures;

		// Extensions
		createInfo.enabledExtensionCount = m_PhysicalDevice->GetDeviceExtensions().size();
		createInfo.ppEnabledExtensionNames = m_PhysicalDevice->GetDeviceExtensions().data();

		// Validation layers
		createInfo.enabledLayerCount = 0;
		const std::vector<const char*>& validationLayers = VulkanRenderContext::GetInstance()->GetValidationLayers();
		if (!validationLayers.empty())
		{
			createInfo.enabledLayerCount = validationLayers.size();
			createInfo.ppEnabledLayerNames = &validationLayers[0];
		}

		// Create device
		VkResult result = vkCreateDevice(m_PhysicalDevice->GetVkPhysicalDevice(), &createInfo, nullptr, &m_Device);
		NB_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan device");

		// Create graphics queue
		vkGetDeviceQueue(m_Device, indices.Graphics.value(), 0, &m_GraphicsQueue);
		vkGetDeviceQueue(m_Device, indices.Presentation.value(), 0, &m_PresentationQueue);
	}
}