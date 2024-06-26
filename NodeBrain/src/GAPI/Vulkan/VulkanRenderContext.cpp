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
				NB_WARN("   {0}", callbackData->pMessage);
			else if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
				NB_ERROR("   {0}", callbackData->pMessage);
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

		static bool CheckInstanceExtensionSupport(const std::vector<const char*>& extensions)
		{
			NB_PROFILE_FN();

			uint32_t availableExtensionCount = 0;
			vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, nullptr);
			std::vector<VkExtensionProperties> availableExtensions(availableExtensionCount);
			vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, &availableExtensions[0]);

			for (auto extension : extensions)
			{
				bool extensionIsAvailable = false;
				for (int j = 0; j < availableExtensionCount; j++)
					if (strcmp(extension, availableExtensions[j].extensionName) == 0)
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
			for (auto validationLayer : validationLayers)
			{
				bool layerIsAvailable = false;
				for (int j = 0; j < layerCount; j++)
					if (strcmp(validationLayer, availableLayers[j].layerName) == 0)
						layerIsAvailable = true;

				if (!layerIsAvailable)
				{
					return false;
				}
			}
			return true;
		}
	}
	


	VulkanRenderContext::VulkanRenderContext(Window& window)
		: m_Window(window)
	{
		NB_PROFILE_FN();

		#ifdef NB_DEBUG
			m_EnabledLayers.push_back("VK_LAYER_KHRONOS_validation");
		#endif

		for (const char* ext : m_Window.GetVulkanExtensions())
			m_EnabledInstanceExtensions.push_back(ext);

		#if NB_APPLE
			m_EnabledInstanceExtensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
			m_EnabledInstanceExtensions.push_back("VK_KHR_get_physical_device_properties2");
		#endif

		VK_CHECK(CreateInstance());

		VK_CHECK(glfwCreateWindowSurface(m_VkInstance, m_Window.GetGLFWWindow(), nullptr, &m_VkSurface));

		#ifdef NB_DEBUG
			VK_CHECK(CreateDebugUtilsMessenger());
		#endif

		m_PhysicalDevice = FindFirstSuitablePhysicalDevice();

		m_Device = std::make_unique<VulkanDevice>(*m_PhysicalDevice, m_EnabledLayers);

		VK_CHECK(CreateImmediateObjects());

		VK_CHECK(CreateAllocator());

		m_Swapchain = std::make_unique<VulkanSwapchain>(m_Window, m_VkSurface, *m_Device, m_VMAAllocator);
		
		VK_CHECK(CreateDescriptorPools());
	}

	VulkanRenderContext::~VulkanRenderContext()
	{
		NB_PROFILE_FN();

		vkDeviceWaitIdle(m_Device->GetVkDevice());

		for (int i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			vkDestroyDescriptorPool(m_Device->GetVkDevice(), m_VkDescriptorPools[i], nullptr);
			m_VkDescriptorPools[i] = VK_NULL_HANDLE;
		}

		m_Swapchain.reset();

		vmaDestroyAllocator(m_VMAAllocator);
		m_VMAAllocator = VK_NULL_HANDLE;

		DestroyImmediateObjects();

		m_Device.reset();

		DestroyDebugUtilsMessenger();

		vkDestroySurfaceKHR(m_VkInstance, m_VkSurface, nullptr);
		m_VkSurface = VK_NULL_HANDLE;

		vkDestroyInstance(m_VkInstance, nullptr);
		m_VkInstance = VK_NULL_HANDLE;
	}

	void VulkanRenderContext::AcquireNextImage()
	{
		NB_PROFILE_FN();

		m_Swapchain->AcquireNextImage();
	}

	void VulkanRenderContext::SwapBuffers()
	{
		NB_PROFILE_FN();

		m_Swapchain->PresentImage();
	}

	void VulkanRenderContext::ImmediateSubmit(const std::function<void(VkCommandBuffer cmdBuffer)>& func)
	{
		NB_PROFILE_FN();

		VK_CHECK(vkResetFences(m_Device->GetVkDevice(), 1, &m_ImmediateFence));
		VK_CHECK(vkResetCommandBuffer(m_ImmediateCmdBuffer, 0));

		VkCommandBuffer cmdBuffer = m_ImmediateCmdBuffer;

		VkCommandBufferBeginInfo cmdBufferBeginInfo = {};
		cmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		VK_CHECK(vkBeginCommandBuffer(cmdBuffer, &cmdBufferBeginInfo));
		func(cmdBuffer);
		VK_CHECK(vkEndCommandBuffer(cmdBuffer));

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &cmdBuffer;

		VK_CHECK(vkQueueSubmit(m_Device->GetGraphicsQueue(), 1, &submitInfo, m_ImmediateFence));

		VK_CHECK(vkWaitForFences(m_Device->GetVkDevice(), 1, &m_ImmediateFence, true, UINT64_MAX));
	}

	void VulkanRenderContext::WaitForGPU()
	{
		NB_PROFILE_FN();

		vkDeviceWaitIdle(m_Device->GetVkDevice());
	}

	VkResult VulkanRenderContext::CreateInstance()
	{
		NB_PROFILE_FN();

		VkApplicationInfo applicationInfo = {};
		applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		applicationInfo.pApplicationName = "NodeBrain";
		applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		applicationInfo.pEngineName = "NodeBrain";
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

	VkResult VulkanRenderContext::CreateDescriptorPools()
	{
		NB_PROFILE_FN();

		for (int i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			uint32_t maxSets = 100;

			std::vector<VkDescriptorPoolSize> poolSizes = 
			{
				{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, maxSets },
				{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, maxSets },
				{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, maxSets },
			};

			VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
			descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			descriptorPoolCreateInfo.flags = 0;
			descriptorPoolCreateInfo.maxSets = maxSets; // TODO: Double check
			descriptorPoolCreateInfo.poolSizeCount = (uint32_t)poolSizes.size();;
			descriptorPoolCreateInfo.pPoolSizes = &poolSizes[0];

			VkResult result = vkCreateDescriptorPool(m_Device->GetVkDevice(), &descriptorPoolCreateInfo, nullptr, &m_VkDescriptorPools[i]);
			if (result != VK_SUCCESS)
				return result;
		}
		
		return VK_SUCCESS;
	}

	VkResult VulkanRenderContext::CreateAllocator()
	{
		NB_PROFILE_FN();

		VmaAllocatorCreateInfo allocatorCreateInfo = {};
		allocatorCreateInfo.physicalDevice = m_PhysicalDevice->GetVkPhysicalDevice();
		allocatorCreateInfo.device = m_Device->GetVkDevice();;
		allocatorCreateInfo.instance = m_VkInstance;
		allocatorCreateInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;

		return vmaCreateAllocator(&allocatorCreateInfo, &m_VMAAllocator);
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

	VkResult VulkanRenderContext::CreateImmediateObjects()
	{
		NB_PROFILE_FN();

		VkResult result;
		
		VkCommandPoolCreateInfo cmdPoolCreateInfo = {};
		cmdPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		cmdPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		cmdPoolCreateInfo.queueFamilyIndex = m_PhysicalDevice->FindQueueFamilies().Graphics.value();
		result = vkCreateCommandPool(m_Device->GetVkDevice(), &cmdPoolCreateInfo, nullptr, &m_ImmediateCmdPool);
		if (result != VK_SUCCESS)
			return result;

		VkCommandBufferAllocateInfo cmdBufferAllocInfo = {};
		cmdBufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		cmdBufferAllocInfo.commandPool = m_ImmediateCmdPool;
		cmdBufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		cmdBufferAllocInfo.commandBufferCount = 1;
		result = vkAllocateCommandBuffers(m_Device->GetVkDevice(), &cmdBufferAllocInfo, &m_ImmediateCmdBuffer);
		if (result != VK_SUCCESS)
			return result;

		VkFenceCreateInfo fenceCreateInfo = {};
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
		result = vkCreateFence(m_Device->GetVkDevice(), &fenceCreateInfo, nullptr, &m_ImmediateFence);
		if (result != VK_SUCCESS)
			return result;

		return VK_SUCCESS;
	}

	std::unique_ptr<VulkanPhysicalDevice> VulkanRenderContext::FindFirstSuitablePhysicalDevice()
	{
		NB_PROFILE_FN();

		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(m_VkInstance, &deviceCount, nullptr);
		NB_ASSERT(deviceCount, "Could not find any GPUs with Vulkan support");
		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(m_VkInstance, &deviceCount, &devices[0]);

		for (int i = 0; i < devices.size(); i++)
		{
			std::unique_ptr<VulkanPhysicalDevice> physicalDevice = std::make_unique<VulkanPhysicalDevice>(m_VkInstance, m_VkSurface, i);
			if (physicalDevice->IsSuitable())
				return physicalDevice;
		}

		NB_ASSERT(false, "Could not find suitable GPU");
		return nullptr;
	}

	void VulkanRenderContext::DestroyDebugUtilsMessenger()
	{
		NB_PROFILE_FN();

		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_VkInstance, "vkDestroyDebugUtilsMessengerEXT");
		NB_ASSERT(func, "Failed to retrieve vkDestroyDebugUtilsMessengerEXT function");
		func(m_VkInstance, m_DebugMessenger, nullptr);
		m_DebugMessenger = VK_NULL_HANDLE;
	}

	void VulkanRenderContext::DestroyImmediateObjects()
	{
		NB_PROFILE_FN();

		vkDestroyCommandPool(m_Device->GetVkDevice(), m_ImmediateCmdPool, nullptr);
		m_ImmediateCmdPool = VK_NULL_HANDLE;
		vkDestroyFence(m_Device->GetVkDevice(), m_ImmediateFence, nullptr);
		m_ImmediateFence = VK_NULL_HANDLE;
	}
}