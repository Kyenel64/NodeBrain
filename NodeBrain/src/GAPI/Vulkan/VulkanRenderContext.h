#pragma once

#include <vulkan/vulkan.h>
#include <VMA/vk_mem_alloc.h>

#include "Core/Window.h"
#include "Renderer/RenderContext.h"
#include "GAPI/Vulkan/VulkanPhysicalDevice.h"
#include "GAPI/Vulkan/VulkanDevice.h"
#include "GAPI/Vulkan/VulkanSwapchain.h"

namespace NodeBrain
{
	class VulkanRenderContext : public RenderContext
	{
	public:
		explicit VulkanRenderContext(Window* window);
		~VulkanRenderContext() override;

		void AcquireNextImage() override;
		void SwapBuffers() override;

		void WaitForGPU() override;

		GAPI GetGraphicsAPI() const override { return GAPI::Vulkan; }
		Window* GetWindow() const override { return m_Window; }

		void ImmediateSubmit(const std::function<void(VkCommandBuffer cmdBuffer)>& func);

		// Getters
		VkInstance GetVkInstance() const { return m_VkInstance; }
		VkDevice GetVkDevice() const { return m_Device->GetVkDevice(); }
		VulkanDevice& GetDevice() const { return *m_Device; }
		VulkanPhysicalDevice& GetPhysicalDevice() const { return *m_PhysicalDevice; }
		VulkanSwapchain& GetSwapchain() const { return *m_Swapchain; }
		VmaAllocator GetVMAAllocator() const { return m_VMAAllocator; }
		const std::vector<const char*>& GetEnabledLayers() const { return m_EnabledLayers; }
		VkDescriptorPool GetVkDescriptorPool() const { return m_VkDescriptorPools[m_Swapchain->GetFrameIndex()]; }

	private:
		VkResult CreateInstance();
		VkResult CreateDescriptorPools();
		VkResult CreateDebugUtilsMessenger();
		VkResult CreateAllocator();
		VkResult CreateImmediateObjects();
		
		void DestroyDebugUtilsMessenger();
		void DestroyImmediateObjects();

		std::unique_ptr<VulkanPhysicalDevice> FindFirstSuitablePhysicalDevice();

	private:
		Window* m_Window = nullptr;
		VkInstance m_VkInstance = VK_NULL_HANDLE;
		VkSurfaceKHR m_VkSurface = VK_NULL_HANDLE;
		VmaAllocator m_VMAAllocator = VK_NULL_HANDLE;
		VkDescriptorPool m_VkDescriptorPools[FRAMES_IN_FLIGHT];
		VkDebugUtilsMessengerEXT m_DebugMessenger = VK_NULL_HANDLE;

		std::unique_ptr<VulkanPhysicalDevice> m_PhysicalDevice;
		std::unique_ptr<VulkanDevice> m_Device;
		std::unique_ptr<VulkanSwapchain> m_Swapchain;

		std::vector<const char*> m_EnabledLayers;
		std::vector<const char*> m_EnabledInstanceExtensions;

		VkFence m_ImmediateFence = VK_NULL_HANDLE;
		VkCommandPool m_ImmediateCmdPool = VK_NULL_HANDLE;
		VkCommandBuffer m_ImmediateCmdBuffer = VK_NULL_HANDLE;
	};
}