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

		static void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
		{
			createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
			createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
			createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
			createInfo.pfnUserCallback = DebugCallback;
		}

		static bool CheckInstanceExtensionSupport(const std::vector<const char*> extensions)
		{
			NB_PROFILE_FN();

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

		static bool CheckValidationLayerSupport(const std::vector<const char*>& validationLayers)
		{
			NB_PROFILE_FN();

			uint32_t layerCount = 0;
			vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
			std::vector<VkLayerProperties> availableLayers(layerCount);
			vkEnumerateInstanceLayerProperties(&layerCount, &availableLayers[0]);

			// Check layers are available
			for (int i = 0; i < validationLayers.size(); i++)
			{
				bool layerIsAvailable = false;
				for (int j = 0; j < layerCount; j++)
					if (strcmp(validationLayers[i], availableLayers[j].layerName) == 0)
						layerIsAvailable = true;

				if (!layerIsAvailable)
				{
					return false;
				}
			}
			return true;
		}
	}
	


	// --- VulkanRenderContext ------------------------------------------------
	static VulkanRenderContext* s_Instance;

	VulkanRenderContext::VulkanRenderContext(Window* window)
		: m_Window(window)
	{
		NB_PROFILE_FN();

		s_Instance = this;

		#ifdef NB_DEBUG
			m_ValidationLayers.push_back("VK_LAYER_KHRONOS_validation");
		#endif

		Init();
	}

	void VulkanRenderContext::Init()
	{
		NB_PROFILE_FN();

		VkResult result = CreateInstance();
		NB_ASSERT(result == VK_SUCCESS, result);

		result = glfwCreateWindowSurface(m_VkInstance, m_Window->GetGLFWWindow(), nullptr, &m_VkSurface);
		NB_ASSERT(result == VK_SUCCESS, result);

		#ifdef NB_DEBUG
			result = CreateDebugUtilsMessenger();
			NB_ASSERT(result == VK_SUCCESS, result);
		#endif

		m_PhysicalDevice = FindFirstSuitablePhysicalDevice();
		m_Device = std::make_shared<VulkanDevice>(m_PhysicalDevice);
		m_SwapChain = std::make_unique<VulkanSwapChain>(m_VkSurface, m_Device);
	}

	VulkanRenderContext::~VulkanRenderContext()
	{
		NB_PROFILE_FN();

		VkFence fence = m_SwapChain->GetInFlightFence();
		vkWaitForFences(m_Device->GetVkDevice(), 1, &fence, VK_TRUE, UINT64_MAX);

		// Destroy in reverse order
		m_SwapChain.reset();

		m_Device.reset();

		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_VkInstance, "vkDestroyDebugUtilsMessengerEXT");
		NB_ASSERT(func, "Could not destroy Vulkan debug messenger");
		func(m_VkInstance, m_DebugMessenger, nullptr);
		m_DebugMessenger = VK_NULL_HANDLE;

		vkDestroySurfaceKHR(m_VkInstance, m_VkSurface, nullptr);
		m_VkSurface = VK_NULL_HANDLE;

		vkDestroyInstance(m_VkInstance, nullptr);
		m_VkInstance = VK_NULL_HANDLE;
	}

	VulkanRenderContext* VulkanRenderContext::GetInstance() 
	{ 
		return s_Instance; 
	}

	std::shared_ptr<VulkanPhysicalDevice> VulkanRenderContext::FindFirstSuitablePhysicalDevice()
	{
		NB_PROFILE_FN();

		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(m_VkInstance, &deviceCount, nullptr);
		NB_ASSERT(deviceCount, "Could not find any GPUs with Vulkan support");
		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(m_VkInstance, &deviceCount, &devices[0]);

		for (int i = 0; i < devices.size(); i++)
		{
			std::shared_ptr<VulkanPhysicalDevice> physicalDevice = std::make_shared<VulkanPhysicalDevice>(m_VkInstance, m_VkSurface, i);
			if (physicalDevice->IsSuitable())
				return physicalDevice;
		}

		NB_ASSERT(false, "Could not find suitable GPU");
		return nullptr;
	}

	VkResult VulkanRenderContext::CreateInstance()
	{
		NB_PROFILE_FN();

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
		for (const char* ext : m_Window->GetVulkanExtensions())
			extensions.push_back(ext);

		#if NB_APPLE
			extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
			extensions.push_back("VK_KHR_get_physical_device_properties2");
			createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
		#endif

		createInfo.enabledExtensionCount = extensions.size();
		createInfo.ppEnabledExtensionNames = &extensions[0];
		createInfo.enabledLayerCount = 0;
		createInfo.pNext = nullptr;

		// Validation layer
		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
		if (!m_ValidationLayers.empty())
		{
			NB_ASSERT(Utils::CheckValidationLayerSupport(m_ValidationLayers), "Validation layers unavailable");
			createInfo.enabledLayerCount = static_cast<uint32_t>(m_ValidationLayers.size());
			createInfo.ppEnabledLayerNames = &m_ValidationLayers[0];

			// Add debug utils extension
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
			createInfo.enabledExtensionCount = extensions.size();
			createInfo.ppEnabledExtensionNames = &extensions[0];

			Utils::PopulateDebugMessengerCreateInfo(debugCreateInfo);
			createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
		}

		NB_ASSERT(Utils::CheckInstanceExtensionSupport(extensions), "Extensions unavailable");

		return vkCreateInstance(&createInfo, nullptr, &m_VkInstance);
	}

	VkResult VulkanRenderContext::CreateDebugUtilsMessenger()
	{
		NB_PROFILE_FN();

		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
		Utils::PopulateDebugMessengerCreateInfo(debugCreateInfo);

		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_VkInstance, "vkCreateDebugUtilsMessengerEXT");
		NB_ASSERT(func, "Failed to create Vulkan debug messenger");

		return func(m_VkInstance, &debugCreateInfo, nullptr, &m_DebugMessenger);
	}

	void VulkanRenderContext::AcquireNextImage()
	{
		m_SwapChain->AcquireNextImage();
	}

	void VulkanRenderContext::SwapBuffers()
	{
		m_SwapChain->PresentImage();
	}
}