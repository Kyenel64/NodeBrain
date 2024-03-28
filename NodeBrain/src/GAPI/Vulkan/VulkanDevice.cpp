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
		
		if (m_VkDevice)
		{
			vkDestroyDevice(m_VkDevice, nullptr);
			m_VkDevice = VK_NULL_HANDLE;
		}
	}

	void VulkanDevice::Init()
	{
		NB_PROFILE_FN();

		// --- Queue info ---
		QueueFamilyIndices indices = m_PhysicalDevice->FindQueueFamilies();
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
		VkPhysicalDeviceFeatures2 deviceFeatures = {}; // TODO:
		deviceFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2_KHR;

		VkPhysicalDeviceVulkan12Features vulkan12Features = {};
		vulkan12Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
		vulkan12Features.bufferDeviceAddress = VK_TRUE;
		vulkan12Features.descriptorIndexing = VK_TRUE;

		VkPhysicalDeviceVulkan13Features vulkan13Features = {};
		vulkan13Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
		vulkan13Features.dynamicRendering = VK_TRUE;
		vulkan13Features.synchronization2 = VK_TRUE;

		//VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamicRenderingFeatures = {};
		//dynamicRenderingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR;
		//dynamicRenderingFeatures.dynamicRendering = VK_TRUE;

		deviceFeatures.pNext = &vulkan12Features;
		#ifdef NB_VULKAN_VERSION_1_3
			vulkan12Features.pNext = &vulkan13Features;
		#else
			vulkan12Features.pNext = VK_NULL_HANDLE;
		#endif

		// --- Device ---
		VkDeviceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.pNext = &deviceFeatures;
		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pQueueCreateInfos = &queueCreateInfos[0];
		//createInfo.pEnabledFeatures = &deviceFeatures;

		// Extensions
		createInfo.enabledExtensionCount = m_PhysicalDevice->GetDeviceExtensions().size();
		createInfo.ppEnabledExtensionNames = m_PhysicalDevice->GetDeviceExtensions().data();

		// Validation layers
		createInfo.enabledLayerCount = 0;
		const std::vector<const char*>& validationLayers = VulkanRenderContext::Get()->GetValidationLayers();
		if (!validationLayers.empty())
		{
			createInfo.enabledLayerCount = validationLayers.size();
			createInfo.ppEnabledLayerNames = &validationLayers[0];
		}

		// Create device
		VK_CHECK(vkCreateDevice(m_PhysicalDevice->GetVkPhysicalDevice(), &createInfo, nullptr, &m_VkDevice));

		// Create graphics queue
		vkGetDeviceQueue(m_VkDevice, indices.Graphics.value(), 0, &m_GraphicsQueue);
		vkGetDeviceQueue(m_VkDevice, indices.Presentation.value(), 0, &m_PresentationQueue);
	}
}