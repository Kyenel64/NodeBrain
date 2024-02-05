#pragma once

#include <vulkan/vulkan.h>

#include "Core/Window.h"

namespace NodeBrain
{
	class VulkanSurface
	{
	public:
		VulkanSurface(Window* window);
		~VulkanSurface();

		VkSurfaceKHR GetVkSurface() const { return m_Surface; }

	private:
		void Init();

	private:
		VkInstance m_VkInstance = VK_NULL_HANDLE;
		VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
		Window* m_Window = nullptr;
	};
}