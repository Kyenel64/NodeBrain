#pragma once

#include <vulkan/vulkan.h>

namespace NodeBrain
{
	class VulkanSurface
	{
	public:
		VulkanSurface(VkInstance instance);
		~VulkanSurface();

		VkSurfaceKHR GetVkSurface() const { return m_Surface; }

	private:
		void Init();

	private:
		VkInstance m_VkInstance = VK_NULL_HANDLE;
		VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
	};
}