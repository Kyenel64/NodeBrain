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
			m_EnabledLayers.push_back("VK_LAYER_KHRONOS_validation");
		#endif

		for (const char* ext : m_Window->GetVulkanExtensions())
			m_EnabledInstanceExtensions.push_back(ext);

		#if NB_APPLE
			m_EnabledInstanceExtensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
			m_EnabledInstanceExtensions.push_back("VK_KHR_get_physical_device_properties2");
		#endif

		Init();
	}

	void VulkanRenderContext::Init()
	{
		NB_PROFILE_FN();

		VK_CHECK(CreateInstance());

		VK_CHECK(glfwCreateWindowSurface(m_VkInstance, m_Window->GetGLFWWindow(), nullptr, &m_VkSurface));

		#ifdef NB_DEBUG
			VK_CHECK(CreateDebugUtilsMessenger());
		#endif

		m_PhysicalDevice = FindFirstSuitablePhysicalDevice();
		m_Device = std::make_shared<VulkanDevice>(m_PhysicalDevice);
		m_Allocator = std::make_unique<VulkanAllocator>();
		m_Swapchain = std::make_unique<VulkanSwapchain>(m_VkSurface, m_Device);

		VK_CHECK(CreateDescriptorPools());
	}

	VkResult VulkanRenderContext::CreateDescriptorPools()
	{
		for (int i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			uint32_t maxSets = 10;
			std::vector<VkDescriptorPoolSize> poolSizes;
			poolSizes.push_back(VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 10 * maxSets});

			VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
			descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			descriptorPoolCreateInfo.flags = 0;
			descriptorPoolCreateInfo.maxSets = maxSets;
			descriptorPoolCreateInfo.poolSizeCount = (uint32_t)poolSizes.size();
			descriptorPoolCreateInfo.pPoolSizes = &poolSizes[0];

			VkResult result = vkCreateDescriptorPool(m_Device->GetVkDevice(), &descriptorPoolCreateInfo, nullptr, &m_VkDescriptorPools[i]);
			if (result != VK_SUCCESS)
				return result;
		}
		
		return VK_SUCCESS;
	}

	VulkanRenderContext::~VulkanRenderContext()
	{
		NB_PROFILE_FN();

		for (int i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			vkDestroyDescriptorPool(m_Device->GetVkDevice(), m_VkDescriptorPools[i], nullptr);
			m_VkDescriptorPools[i] = VK_NULL_HANDLE;
		}

		m_Swapchain.reset();
		m_Allocator.reset();
		m_Device.reset();

		DestroyDebugUtilsMessenger();

		vkDestroySurfaceKHR(m_VkInstance, m_VkSurface, nullptr);
		m_VkSurface = VK_NULL_HANDLE;

		vkDestroyInstance(m_VkInstance, nullptr);
		m_VkInstance = VK_NULL_HANDLE;
	}

	VulkanRenderContext* VulkanRenderContext::Get() 
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

		VkApplicationInfo applicationInfo = {};
		applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		applicationInfo.pApplicationName = App::Get()->GetApplicationName().c_str();
		applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		applicationInfo.pEngineName = App::Get()->GetApplicationName().c_str();
		applicationInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);
		applicationInfo.apiVersion = VK_API_VERSION_1_2;

		VkInstanceCreateInfo instanceCreateInfo = {};
		instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instanceCreateInfo.pApplicationInfo = &applicationInfo;

		instanceCreateInfo.enabledExtensionCount = m_EnabledInstanceExtensions.size();
		instanceCreateInfo.ppEnabledExtensionNames = &m_EnabledInstanceExtensions[0];
		instanceCreateInfo.enabledLayerCount = 0;
		instanceCreateInfo.pNext = nullptr;

		#ifdef NB_APPLE
			instanceCreateInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
		#endif

		// Validation layer
		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
		if (!m_EnabledLayers.empty())
		{
			NB_ASSERT(Utils::CheckValidationLayerSupport(m_EnabledLayers), "Enabled layers are unavailable");
			instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(m_EnabledLayers.size());
			instanceCreateInfo.ppEnabledLayerNames = &m_EnabledLayers[0];

			// Add debug utils extension
			m_EnabledInstanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
			instanceCreateInfo.enabledExtensionCount = m_EnabledInstanceExtensions.size();
			instanceCreateInfo.ppEnabledExtensionNames = &m_EnabledInstanceExtensions[0];

			Utils::PopulateDebugMessengerCreateInfo(debugCreateInfo);
			instanceCreateInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
		}

		NB_ASSERT(Utils::CheckInstanceExtensionSupport(m_EnabledInstanceExtensions), "Unavailable instance extensions enabled");

		return vkCreateInstance(&instanceCreateInfo, nullptr, &m_VkInstance);
	}

	VkResult VulkanRenderContext::CreateDebugUtilsMessenger()
	{
		NB_PROFILE_FN();

		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
		Utils::PopulateDebugMessengerCreateInfo(debugCreateInfo);

		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_VkInstance, "vkCreateDebugUtilsMessengerEXT");
		NB_ASSERT(func, "Failed to retrieve vkCreateDebugUtilsMessengerEXT function");

		return func(m_VkInstance, &debugCreateInfo, nullptr, &m_DebugMessenger);
	}

	void VulkanRenderContext::DestroyDebugUtilsMessenger()
	{
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_VkInstance, "vkDestroyDebugUtilsMessengerEXT");
		NB_ASSERT(func, "Failed to retrieve vkDestroyDebugUtilsMessengerEXT function");
		func(m_VkInstance, m_DebugMessenger, nullptr);
		m_DebugMessenger = VK_NULL_HANDLE;
	}

	void VulkanRenderContext::AcquireNextImage()
	{
		m_Swapchain->AcquireNextImage();
	}

	void VulkanRenderContext::SwapBuffers()
	{
		m_Swapchain->PresentImage();
	}
}