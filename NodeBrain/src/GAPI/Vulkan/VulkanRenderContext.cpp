#include "NBpch.h"
#include "VulkanRenderContext.h"

#include <GLFW/glfw3.h>

#include "Core/App.h"

namespace NodeBrain
{
	namespace Utils
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
	}
	

	static VulkanRenderContext* s_Instance;

	VulkanRenderContext::VulkanRenderContext(Window* window)
		: m_Window(window)
	{
		s_Instance = this;

		#ifdef NB_DEBUG
			m_EnableValidationLayers = true;
			m_ValidationLayers.push_back("VK_LAYER_KHRONOS_validation");
		#endif

		Init();
	}

	VulkanRenderContext* VulkanRenderContext::GetInstance() 
	{ 
		return s_Instance; 
	}

	VulkanRenderContext::~VulkanRenderContext()
	{
		m_SwapChain.reset();
		m_Device.reset();
		m_PhysicalDevice.reset();
		m_Surface.reset();
		Utils::DestroyDebugUtilsMessengerEXT(m_VkInstance, m_DebugMessenger, nullptr);
		vkDestroyInstance(m_VkInstance, nullptr);
	}

	void VulkanRenderContext::Init()
	{
		VkResult result = CreateInstance();
		NB_ASSERT(result == VK_SUCCESS, result);

		m_Surface = std::make_shared<VulkanSurface>(m_Window);

		result = CreateDebugUtilsMessenger();
		NB_ASSERT(result == VK_SUCCESS, result);

		m_PhysicalDevice = std::make_shared<VulkanPhysicalDevice>(0);
		m_Device = std::make_shared<VulkanDevice>(m_PhysicalDevice);
		m_SwapChain = std::make_shared<VulkanSwapChain>(m_Device);
	}

	VkResult VulkanRenderContext::CreateInstance()
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
		for (const char* ext : m_Window->GetExtensions())
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
		{
			NB_ASSERT(CheckValidationLayerSupport(), "Validation layers unavailable");
			createInfo.enabledLayerCount = static_cast<uint32_t>(m_ValidationLayers.size());
			createInfo.ppEnabledLayerNames = &m_ValidationLayers[0];

			// Add debug utils extension
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
			createInfo.enabledExtensionCount = extensions.size();
			createInfo.ppEnabledExtensionNames = &extensions[0];

			Utils::PopulateDebugMessengerCreateInfo(debugCreateInfo);
			createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
		}

		CheckExtensionSupport(extensions);

		return vkCreateInstance(&createInfo, nullptr, &m_VkInstance);
	}

	VkResult VulkanRenderContext::CreateDebugUtilsMessenger()
	{
		VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
		Utils::PopulateDebugMessengerCreateInfo(createInfo);

		return Utils::CreateDebugUtilsMessengerEXT(m_VkInstance, &createInfo, nullptr, &m_DebugMessenger);
	}

	bool VulkanRenderContext::CheckExtensionSupport(const std::vector<const char*> extensions)
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

	bool VulkanRenderContext::CheckValidationLayerSupport()
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

	void VulkanRenderContext::SwapBuffers()
	{
		
	}
}