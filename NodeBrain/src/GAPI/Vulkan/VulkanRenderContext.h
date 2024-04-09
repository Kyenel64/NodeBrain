#pragma once

#include <vulkan/vulkan.h>

#include "Core/Window.h"
#include "Renderer/RenderContext.h"
#include "GAPI/Vulkan/VulkanPhysicalDevice.h"
#include "GAPI/Vulkan/VulkanDevice.h"
#include "GAPI/Vulkan/VulkanSwapchain.h"
#include "GAPI/Vulkan/VulkanAllocator.h"
#include "GAPI/Vulkan/VulkanDescriptorPool.h"

namespace NodeBrain
{
	class VulkanRenderContext : public RenderContext
	{
	public:
		VulkanRenderContext(Window* window);
		~VulkanRenderContext();

		virtual void Init() override;
		virtual void AcquireNextImage() override;
		virtual void SwapBuffers() override;

		// Getters
		VkInstance GetVkInstance() const { return m_VkInstance; }
		std::shared_ptr<VulkanDevice> GetDevice() const { return m_Device; }
		std::shared_ptr<VulkanPhysicalDevice> GetPhysicalDevice() const { return m_PhysicalDevice; }
		VulkanSwapchain& GetSwapchain() const { return *m_Swapchain; }
		VulkanAllocator& GetAllocator() const { return *m_Allocator; }
		const std::vector<const char*>& GetEnabledLayers() const { return m_EnabledLayers; }
		VkDescriptorPool GetVkDescriptorPool() const { return m_DescriptorPool->GetVkDescriptorPool(); }

		static VulkanRenderContext* Get();

	private:
		VkResult CreateInstance();
		VkResult CreateDebugUtilsMessenger();
		void DestroyDebugUtilsMessenger();

		std::shared_ptr<VulkanPhysicalDevice> FindFirstSuitablePhysicalDevice();

	private:
		Window* m_Window = nullptr;
		VkInstance m_VkInstance = VK_NULL_HANDLE;
		VkSurfaceKHR m_VkSurface = VK_NULL_HANDLE;

		std::shared_ptr<VulkanPhysicalDevice> m_PhysicalDevice; // TODO: make unique
		std::shared_ptr<VulkanDevice> m_Device;
		std::unique_ptr<VulkanSwapchain> m_Swapchain;
		std::unique_ptr<VulkanAllocator> m_Allocator;
		std::unique_ptr<VulkanDescriptorPool> m_DescriptorPool;

		std::vector<const char*> m_EnabledLayers;
		std::vector<const char*> m_EnabledInstanceExtensions;

		VkDebugUtilsMessengerEXT m_DebugMessenger = nullptr;
	};
}