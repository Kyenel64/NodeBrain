#include "NBpch.h"
#include "VulkanSurface.h"

#include "GLFW/glfw3.h"

#include "Core/App.h"

namespace NodeBrain
{
	VulkanSurface::VulkanSurface(VkInstance instance)
		: m_VkInstance(instance)
	{
		Init();
	}

	VulkanSurface::~VulkanSurface()
	{
		vkDestroySurfaceKHR(m_VkInstance, m_Surface, nullptr);
	}

	void VulkanSurface::Init()
	{
		VkResult result = glfwCreateWindowSurface(m_VkInstance, App::GetInstance()->GetWindow().GetGLFWWindow(), nullptr, &m_Surface);
		NB_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan surface");
	}
}