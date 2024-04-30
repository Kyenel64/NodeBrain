#include "NBpch.h"
#include "VulkanDevice.h"

#include "GAPI/Vulkan/VulkanRenderContext.h"

namespace NodeBrain
{
	VulkanDevice::VulkanDevice(VulkanPhysicalDevice& physicalDevice)
		: m_PhysicalDevice(physicalDevice)
	{
		NB_PROFILE_FN();

		// --- Queue info ---
		QueueFamilyIndices indices = m_PhysicalDevice.FindQueueFamilies();
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
		VkPhysicalDeviceFeatures2 deviceFeatures = {};
		deviceFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2_KHR;

		VkPhysicalDeviceVulkan12Features vulkan12Features = {};
		vulkan12Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
		vulkan12Features.bufferDeviceAddress = VK_TRUE;
		vulkan12Features.descriptorIndexing = VK_TRUE;

		VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamicRenderingFeatures = {};
		dynamicRenderingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR;
		dynamicRenderingFeatures.dynamicRendering = VK_TRUE;

		VkPhysicalDeviceSynchronization2FeaturesKHR synchronization2Features = {};
		synchronization2Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES_KHR;
		synchronization2Features.synchronization2 = VK_TRUE;

		deviceFeatures.pNext = &vulkan12Features;
		vulkan12Features.pNext = &dynamicRenderingFeatures;
		dynamicRenderingFeatures.pNext = &synchronization2Features;


		// --- Device ---
		VkDeviceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.pNext = &deviceFeatures;
		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pQueueCreateInfos = &queueCreateInfos[0];
		createInfo.enabledExtensionCount = m_PhysicalDevice.GetEnabledDeviceExtensions().size();
		createInfo.ppEnabledExtensionNames = m_PhysicalDevice.GetEnabledDeviceExtensions().data();

		// Validation layers
		createInfo.enabledLayerCount = 0;
		const std::vector<const char*>& validationLayers = VulkanRenderContext::Get()->GetEnabledLayers();
		if (!validationLayers.empty())
		{
			createInfo.enabledLayerCount = validationLayers.size();
			createInfo.ppEnabledLayerNames = &validationLayers[0];
		}

		VK_CHECK(vkCreateDevice(m_PhysicalDevice.GetVkPhysicalDevice(), &createInfo, nullptr, &m_VkDevice));

		// Retrieve queues
		vkGetDeviceQueue(m_VkDevice, indices.Graphics.value(), 0, &m_GraphicsQueue);
		vkGetDeviceQueue(m_VkDevice, indices.Presentation.value(), 0, &m_PresentationQueue);
	}

	VulkanDevice::~VulkanDevice()
	{
		NB_PROFILE_FN();
		
		vkDestroyDevice(m_VkDevice, nullptr);
		m_VkDevice = VK_NULL_HANDLE;
	}
}