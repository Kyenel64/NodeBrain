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

		static void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& debugCreateInfo)
		{
			NB_PROFILE_FN();

			debugCreateInfo = {};
			debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
			debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
			debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
			debugCreateInfo.pfnUserCallback = DebugCallback;
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

		static bool CheckValidationLayerSupport(const std::vector<const char*> validationLayers)
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

		static QueueFamilyIndices FindPhysicalDeviceQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface)
		{
			QueueFamilyIndices indices = {};
			uint32_t queueFamilyCount = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
			std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
			vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, &queueFamilies[0]);

			for (int i = 0; i < queueFamilyCount; i++)
			{
				if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
				{
					VkBool32 presentationSupport = false;
					vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentationSupport);
					if (presentationSupport)
						indices.Presentation = i;

					indices.Graphics = i;
				}
			}

			return indices;
		}

		static bool CheckPhysicalDeviceExtensionSupport(VkPhysicalDevice device,const std::vector<const char*>& extensions)
		{
			NB_PROFILE_FN();

			uint32_t extensionCount = 0;
			vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
			std::vector<VkExtensionProperties> availableExtensions(extensionCount);
			vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, &availableExtensions[0]);

			std::set<std::string> requiredExtensions(extensions.begin(), extensions.end());

			for (const auto& extension : availableExtensions)
				requiredExtensions.erase(extension.extensionName);

			return requiredExtensions.empty();
		}

		static SwapChainSupportDetails QueryPhysicalDeviceSwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface)
		{
			NB_PROFILE_FN();

			// Capabilities
			SwapChainSupportDetails supportDetails = {};
			vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &supportDetails.Capabilities);

			// Formats
			uint32_t formatCount = 0;
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
			if (formatCount)
			{
				supportDetails.Formats.resize(formatCount);
				vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, supportDetails.Formats.data());
			}

			// Presentation Modes
			uint32_t presentationModeCount = 0;
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentationModeCount, nullptr);
			if (presentationModeCount)
			{
				supportDetails.PresentationModes.resize(presentationModeCount);
				vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentationModeCount, supportDetails.PresentationModes.data());
			}

			return supportDetails;
		}

		static bool IsPhysicalDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface, const std::vector<const char*>& extensions)
		{
			if (!device)
				return false;
			QueueFamilyIndices indices = FindPhysicalDeviceQueueFamilies(device, surface);
			bool extensionSupported = CheckPhysicalDeviceExtensionSupport(device, extensions);

			bool swapchainAdequate = false;
			if (extensionSupported)
			{
				SwapChainSupportDetails swapchainSupport = QueryPhysicalDeviceSwapChainSupport(device, surface);
				swapchainAdequate = !swapchainSupport.Formats.empty() && !swapchainSupport.PresentationModes.empty();
			}
			return indices.IsComplete() && extensionSupported && swapchainAdequate;
		}

		static VkSurfaceFormatKHR ChooseSwapChainFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
		{
			NB_PROFILE_FN();

			for (const auto& format : availableFormats)
			{
				if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR)
					return format;
			}

			return availableFormats[0];
		}

		static VkPresentModeKHR ChooseSwapChainPresentationMode(const std::vector<VkPresentModeKHR>& availablePresentationModes)
		{
			NB_PROFILE_FN();

			for (const auto& presentationMode : availablePresentationModes)
			{
				if (presentationMode == VK_PRESENT_MODE_MAILBOX_KHR)
					return presentationMode;
			}
			return VK_PRESENT_MODE_FIFO_KHR;
		}

		static VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
		{
			NB_PROFILE_FN();

			if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
			{
				return capabilities.currentExtent;
			}
			else
			{
				glm::vec2 framebufferSize = App::GetInstance()->GetWindow().GetFramebufferSize();
				VkExtent2D actualExtent = { static_cast<uint32_t>(framebufferSize.x), static_cast<uint32_t>(framebufferSize.y) };
				actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
				actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

				return actualExtent;
			}
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

		m_PhysicalDeviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
		#if NB_APPLE
			m_PhysicalDeviceExtensions.push_back("VK_KHR_portability_subset");
		#endif

		Init();
	}

	VulkanRenderContext::~VulkanRenderContext()
	{
		NB_PROFILE_FN();

		// Destroy in reverse order
		// Image views
		for (auto imageView : m_SwapchainImageViews) 
        	vkDestroyImageView(m_VkDevice, imageView, nullptr);

		// Swapchain
		vkDestroySwapchainKHR(m_VkDevice, m_VkSwapchain, nullptr);
		m_VkSwapchain = VK_NULL_HANDLE;

		// Device
		vkDestroyDevice(m_VkDevice, nullptr);
		m_VkDevice = VK_NULL_HANDLE;
		
		// Debug messenger
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_VkInstance, "vkDestroyDebugUtilsMessengerEXT");
		NB_ASSERT(func, "Failed to destroy Vulkan debug messenger");
		func(m_VkInstance, m_DebugMessenger, nullptr);
		m_DebugMessenger = VK_NULL_HANDLE;

		// Surface
		vkDestroySurfaceKHR(m_VkInstance, m_VkSurface, nullptr);
		m_VkSurface = VK_NULL_HANDLE;

		// Instance
		vkDestroyInstance(m_VkInstance, nullptr);
		m_VkInstance = VK_NULL_HANDLE;
	}

	void VulkanRenderContext::Init()
	{
		NB_PROFILE_FN();

		CreateInstance();

		VkResult result = glfwCreateWindowSurface(m_VkInstance, m_Window->GetGLFWWindow(), nullptr, &m_VkSurface);
		NB_ASSERT(result == VK_SUCCESS, result);

		CreateDebugUtilsMessenger();

		CreatePhysicalDevice();
		CreateDevice();
		CreateSwapchain();
	}

	VulkanRenderContext* VulkanRenderContext::GetInstance() 
	{ 
		return s_Instance; 
	}

	void VulkanRenderContext::CreateInstance()
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
		VkInstanceCreateInfo instanceCreateInfo = {};
		instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instanceCreateInfo.pApplicationInfo = &appInfo;

		// Extensions
		std::vector<const char*> extensions;
		for (const char* ext : m_Window->GetExtensions())
			extensions.push_back(ext);

		#if NB_APPLE
			extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
			extensions.push_back("VK_KHR_get_physical_device_properties2");
			instanceCreateInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
		#endif

		instanceCreateInfo.enabledExtensionCount = extensions.size();
		instanceCreateInfo.ppEnabledExtensionNames = &extensions[0];
		instanceCreateInfo.enabledLayerCount = 0;
		instanceCreateInfo.pNext = nullptr;

		// Validation layer
		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
		if (!m_ValidationLayers.empty())
		{
			NB_ASSERT(Utils::CheckValidationLayerSupport(m_ValidationLayers), "Validation layers unavailable");
			instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(m_ValidationLayers.size());
			instanceCreateInfo.ppEnabledLayerNames = &m_ValidationLayers[0];

			// Add debug utils extension
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
			instanceCreateInfo.enabledExtensionCount = extensions.size();
			instanceCreateInfo.ppEnabledExtensionNames = &extensions[0];

			Utils::PopulateDebugMessengerCreateInfo(debugCreateInfo);
			instanceCreateInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
		}

		NB_ASSERT(Utils::CheckInstanceExtensionSupport(extensions), "Vulkan instance extensions not supported!");

		VkResult result = vkCreateInstance(&instanceCreateInfo, nullptr, &m_VkInstance);
		NB_ASSERT(result == VK_SUCCESS, result);
	}

	void VulkanRenderContext::CreatePhysicalDevice()
	{
		NB_PROFILE_FN();

		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(m_VkInstance, &deviceCount, nullptr);
		NB_ASSERT(deviceCount, "Could not find any GPUs with Vulkan support");
		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(m_VkInstance, &deviceCount, &devices[0]);

		for (auto device : devices)
		{
			if (Utils::IsPhysicalDeviceSuitable(device, m_VkSurface, m_PhysicalDeviceExtensions))
				m_VkPhysicalDevice = device;
		}

		NB_ASSERT(m_VkPhysicalDevice, "Could not find suitable GPU");

		m_QueueFamilyIndices = Utils::FindPhysicalDeviceQueueFamilies(m_VkPhysicalDevice, m_VkSurface);
		m_SwapChainSupportDetails = Utils::QueryPhysicalDeviceSwapChainSupport(m_VkPhysicalDevice, m_VkSurface);
	}

	void VulkanRenderContext::CreateDevice()
	{
		NB_PROFILE_FN();

		// --- Queue info ---
		std::vector<VkDeviceQueueCreateInfo> deviceQueueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = { m_QueueFamilyIndices.Graphics.value(), m_QueueFamilyIndices.Presentation.value() };
		float queuePriority = 1.0f;
		for (uint32_t queueFamily : uniqueQueueFamilies)
		{
			VkDeviceQueueCreateInfo deviceQueueCreateInfo = {};
			deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			deviceQueueCreateInfo.queueFamilyIndex = queueFamily;
			deviceQueueCreateInfo.queueCount = 1;
			deviceQueueCreateInfo.pQueuePriorities = &queuePriority;
			deviceQueueCreateInfos.push_back(deviceQueueCreateInfo);
		}
		
		// --- Features ---
		VkPhysicalDeviceFeatures deviceFeatures = {}; // TODO:

		// --- Device ---
		VkDeviceCreateInfo deviceCreateInfo = {};
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(deviceQueueCreateInfos.size());
		deviceCreateInfo.pQueueCreateInfos = &deviceQueueCreateInfos[0];
		deviceCreateInfo.pEnabledFeatures = &deviceFeatures;

		// Extensions
		deviceCreateInfo.enabledExtensionCount = m_PhysicalDeviceExtensions.size();
		deviceCreateInfo.ppEnabledExtensionNames = &m_PhysicalDeviceExtensions[0];

		// Validation layers
		deviceCreateInfo.enabledLayerCount = 0;
		if (!m_ValidationLayers.empty())
		{
			deviceCreateInfo.enabledLayerCount = m_ValidationLayers.size();
			deviceCreateInfo.ppEnabledLayerNames = &m_ValidationLayers[0];
		}

		// Create device
		VkResult result = vkCreateDevice(m_VkPhysicalDevice, &deviceCreateInfo, nullptr, &m_VkDevice);
		NB_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan device");

		// Create graphics queue
		vkGetDeviceQueue(m_VkDevice, m_QueueFamilyIndices.Graphics.value(), 0, &m_GraphicsQueue);
		vkGetDeviceQueue(m_VkDevice, m_QueueFamilyIndices.Presentation.value(), 0, &m_PresentationQueue);
	}

	void VulkanRenderContext::CreateSwapchain()
	{
		NB_PROFILE_FN();

		// Set configurations
		VkSurfaceFormatKHR surfaceFormat = Utils::ChooseSwapChainFormat(m_SwapChainSupportDetails.Formats);
		m_ColorFormat = surfaceFormat.format;
		m_ColorSpace = surfaceFormat.colorSpace;
		m_PresentationMode = Utils::ChooseSwapChainPresentationMode(m_SwapChainSupportDetails.PresentationModes);
		m_Extent = Utils::ChooseSwapExtent(m_SwapChainSupportDetails.Capabilities);

		uint32_t imageCount = m_SwapChainSupportDetails.Capabilities.minImageCount + 1;
		if (m_SwapChainSupportDetails.Capabilities.maxImageCount > 0 && imageCount > m_SwapChainSupportDetails.Capabilities.maxImageCount)
			imageCount = m_SwapChainSupportDetails.Capabilities.maxImageCount;

		// --- Create info ---
		VkSwapchainCreateInfoKHR swapchainCreateInfo{};
		swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapchainCreateInfo.surface = m_VkSurface;
		swapchainCreateInfo.minImageCount = imageCount;
		swapchainCreateInfo.imageFormat = m_ColorFormat;
		swapchainCreateInfo.imageColorSpace = m_ColorSpace;
		swapchainCreateInfo.imageExtent = m_Extent;
		swapchainCreateInfo.imageArrayLayers = 1;
		swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		uint32_t queueFamilyIndices[] = { m_QueueFamilyIndices.Graphics.value(), m_QueueFamilyIndices.Presentation.value() };

		if (m_QueueFamilyIndices.Graphics != m_QueueFamilyIndices.Presentation) 
		{
			swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			swapchainCreateInfo.queueFamilyIndexCount = 2;
			swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else
		{
			swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			swapchainCreateInfo.queueFamilyIndexCount = 0;
			swapchainCreateInfo.pQueueFamilyIndices = nullptr;
		}
		swapchainCreateInfo.preTransform = m_SwapChainSupportDetails.Capabilities.currentTransform;
		swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		swapchainCreateInfo.presentMode = m_PresentationMode;
		swapchainCreateInfo.clipped = VK_TRUE;
		swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

		VkResult result = vkCreateSwapchainKHR(m_VkDevice, &swapchainCreateInfo, nullptr, &m_VkSwapchain);
		NB_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan swap chain");

		std::vector<VkImage> m_SwapchainImages;
		vkGetSwapchainImagesKHR(m_VkDevice, m_VkSwapchain, &imageCount, nullptr);
		m_SwapchainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(m_VkDevice, m_VkSwapchain, &imageCount, &m_SwapchainImages[0]);
	}

	void VulkanRenderContext::CreateImageViews()
	{
		NB_PROFILE_FN();
		m_SwapchainImageViews.resize(m_SwapchainImages.size());

		for (size_t i = 0; i < m_SwapchainImages.size(); i++)
		{
			VkImageViewCreateInfo imageViewCreateInfo = {};
			imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			imageViewCreateInfo.image = m_SwapchainImages[i];
			imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D; // TODO: parameterize
			imageViewCreateInfo.format = m_ColorFormat;

			// TODO: parameterize
			imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
			imageViewCreateInfo.subresourceRange.levelCount = 1;
			imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
			imageViewCreateInfo.subresourceRange.layerCount = 1;

			VkResult result = vkCreateImageView(m_VkDevice, &imageViewCreateInfo, nullptr, &m_SwapchainImageViews[i]);
			NB_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan image view");
		}
	}

	void VulkanRenderContext::CreateDebugUtilsMessenger()
	{
		NB_PROFILE_FN();

		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
		Utils::PopulateDebugMessengerCreateInfo(debugCreateInfo);

		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_VkInstance, "vkCreateDebugUtilsMessengerEXT");
		NB_ASSERT(func, "Failed to create Vulkan debug messenger");
		func(m_VkInstance, &debugCreateInfo, nullptr, &m_DebugMessenger);
	}

	void VulkanRenderContext::SwapBuffers()
	{
		
	}
}