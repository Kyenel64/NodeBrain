#pragma once

#include <vulkan/vulkan.h>

#include "Renderer/Framebuffer.h"
#include "GAPI/Vulkan/VulkanRenderPass.h"
#include "GAPI/Vulkan/VulkanSwapChain.h"
#include "GAPI/Vulkan/VulkanDevice.h"
#include "GAPI/Vulkan/VulkanImage.h"

namespace NodeBrain
{
	class VulkanFramebuffer : public Framebuffer
	{
	public:
		VulkanFramebuffer(std::shared_ptr<VulkanRenderPass> renderPass, std::shared_ptr<VulkanImage> image);
		virtual ~VulkanFramebuffer();

		VkFramebuffer GetVkFramebuffer() const { return m_VkFramebuffer; }

	private:
		void Init();

	private:
		VkFramebuffer m_VkFramebuffer = VK_NULL_HANDLE;
		std::shared_ptr<VulkanRenderPass> m_RenderPass;
		std::shared_ptr<VulkanDevice> m_Device;

		std::shared_ptr<VulkanImage> m_Image;
	};
}