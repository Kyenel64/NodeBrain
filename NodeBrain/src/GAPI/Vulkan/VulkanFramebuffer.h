#pragma once

#include <vulkan/vulkan.h>

#include "Renderer/Framebuffer.h"

namespace NodeBrain
{
	class VulkanFramebuffer : public Framebuffer
	{
	public:
		VulkanFramebuffer(const FramebufferConfiguration& configuration);
		virtual ~VulkanFramebuffer();

		virtual const FramebufferConfiguration& GetConfiguration() const override { return m_Configuration; }
		VkFramebuffer GetVkFramebuffer() const { return m_VkFramebuffer; }

	private:
		void Init();

	private:
		VkFramebuffer m_VkFramebuffer = VK_NULL_HANDLE;
		
		FramebufferConfiguration m_Configuration;
	};
}