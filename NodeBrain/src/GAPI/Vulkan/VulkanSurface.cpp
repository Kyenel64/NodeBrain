#include "NBpch.h"
#include "VulkanSurface.h"

#include "GLFW/glfw3.h"

#include "GAPI/Vulkan/VulkanRenderContext.h"

namespace NodeBrain
{
	VulkanSurface::VulkanSurface(Window* window)
		: m_Window(window)
	{
		NB_PROFILE_FN();

		m_VkInstance = VulkanRenderContext::GetInstance()->GetVkInstance();
		Init();
	}

	VulkanSurface::~VulkanSurface()
	{
		NB_PROFILE_FN();

		vkDestroySurfaceKHR(m_VkInstance, m_Surface, nullptr);
	}

	void VulkanSurface::Init()
	{
		NB_PROFILE_FN();

		VkResult result = glfwCreateWindowSurface(m_VkInstance, m_Window->GetGLFWWindow(), nullptr, &m_Surface);
		NB_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan surface");
	}
}