#include "NBpch.h"
#include "ValidationLayer.h"

namespace NodeBrain
{
	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
		VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT* callbackData, void* userData)
	{
		if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
			NB_WARN("Validation warning: {0}", callbackData->pMessage);
		else if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
			NB_ERROR("Validation error: {0}", callbackData->pMessage);
		return VK_FALSE;
	}

	static VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
		const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
	{
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
		if (func != nullptr)
			return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
		else
			return VK_ERROR_EXTENSION_NOT_PRESENT;
	}

	static void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
	{
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		if (func)
			func(instance, debugMessenger, pAllocator);
	}

	static void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
	{
		createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = DebugCallback;
	}

	ValidationLayer::ValidationLayer()
	{
		m_ValidationLayers.push_back("VK_LAYER_KHRONOS_validation");
	}

	ValidationLayer::~ValidationLayer()
	{
		DestroyDebugUtilsMessengerEXT(m_VkInstance, m_DebugMessenger, nullptr);
	}

	void ValidationLayer::Setup(VkInstance instance)
	{
		m_VkInstance = instance;

		VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
		PopulateDebugMessengerCreateInfo(createInfo);

		VkResult result = CreateDebugUtilsMessengerEXT(m_VkInstance, &createInfo, nullptr, &m_DebugMessenger);
		NB_ASSERT(result == VK_SUCCESS, "Failed to setup debug messenger");
	}

	bool ValidationLayer::CheckValidationLayerSupport()
	{
		uint32_t layerCount = 0;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, &availableLayers[0]);

		// Check layers are available
		for (int i = 0; i < m_ValidationLayers.size(); i++)
		{
			bool layerIsAvailable = false;
			for (int j = 0; j < layerCount; j++)
				if (strcmp(m_ValidationLayers[i], availableLayers[j].layerName) == 0)
					layerIsAvailable = true;

			if (!layerIsAvailable)
			{
				return false;
			}
		}

		return true;
	}

	void ValidationLayer::PopulateCreateInfo(VkInstanceCreateInfo& createInfo, std::vector<const char*>& extensions, VkDebugUtilsMessengerCreateInfoEXT& debugCreateInfo)
	{
		NB_ASSERT(CheckValidationLayerSupport(), "Validation layers unavailable");
		createInfo.enabledLayerCount = static_cast<uint32_t>(m_ValidationLayers.size());
		createInfo.ppEnabledLayerNames = &m_ValidationLayers[0];

		// Add debug utils extension
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		createInfo.enabledExtensionCount = extensions.size();
		createInfo.ppEnabledExtensionNames = &extensions[0];

		PopulateDebugMessengerCreateInfo(debugCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
	}
}