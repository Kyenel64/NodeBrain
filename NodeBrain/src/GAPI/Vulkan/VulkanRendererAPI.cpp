#include "NBpch.h"
#include "VulkanRendererAPI.h"

#include "Core/App.h"

#define NB_LIST_AVAILABLE_VK_EXTENSTIONS

namespace NodeBrain
{
	VulkanRendererAPI::VulkanRendererAPI()
	{
		Init();
	}

	void VulkanRendererAPI::Init()
	{
		#ifdef NB_DEBUG
			m_EnableValidationLayers = true;
			m_ValidationLayer = std::make_unique<VulkanValidationLayer>();
		#endif

		VkResult result = CreateInstance();
		NB_ASSERT(result == VK_SUCCESS, result);

		if (m_EnableValidationLayers)
			m_ValidationLayer->Setup(m_VkInstance);

		m_PhysicalDevice = std::make_shared<VulkanPhysicalDevice>(m_VkInstance);
	}

	VulkanRendererAPI::~VulkanRendererAPI()
	{
		m_ValidationLayer.reset(); // Destroy validation layer before instance
		vkDestroyInstance(m_VkInstance, nullptr);
	}

	VkResult VulkanRendererAPI::CreateInstance()
	{
		// --- App info ---
		VkApplicationInfo appInfo = {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "BrainEditor"; // TODO:
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "NodeBrain";
		appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1); // TODO:
		appInfo.apiVersion = VK_API_VERSION_1_0;

		// --- Create info ---
		VkInstanceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		// Extensions
		std::vector<const char*> extensions;
		for (const char* ext : App::GetInstance()->GetWindow().GetExtensions())
			extensions.push_back(ext);

		#if NB_APPLE
			extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
			createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
		#endif

		createInfo.enabledExtensionCount = extensions.size();
		createInfo.ppEnabledExtensionNames = &extensions[0];
		createInfo.enabledLayerCount = 0;
		createInfo.pNext = nullptr;

		// Validation layer
		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
		if (m_EnableValidationLayers)
			m_ValidationLayer->PopulateCreateInfo(createInfo, extensions, debugCreateInfo);

		CheckExtensionSupport(extensions);

		return vkCreateInstance(&createInfo, nullptr, &m_VkInstance);
	}

	bool VulkanRendererAPI::CheckExtensionSupport(const std::vector<const char*> extensions)
	{
		uint32_t availableExtensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, nullptr);
		std::vector<VkExtensionProperties> availableExtensions(availableExtensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, &availableExtensions[0]);

		for (int i = 0; i < extensions.size(); i++)
		{
			bool extensionIsAvailable = false;
			for (int j = 0; j < availableExtensionCount; j++)
				if (strcmp(extensions[i], availableExtensions[j].extensionName) == 0)
					extensionIsAvailable = true;

			if (!extensionIsAvailable)
				return false;
		}
		
		#ifdef NB_LIST_AVAILABLE_VK_EXTENSTIONS
			NB_INFO("Available Vulkan Extensions:");
			for (int i = 0; i < availableExtensionCount; i++)
				NB_INFO("\t{0}", availableExtensions[i].extensionName);
		#endif

		return true;
	}
}