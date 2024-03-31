#pragma once

#include "Renderer/RenderPass.h"
#include "GAPI/Vulkan/VulkanFramebuffer.h"

namespace NodeBrain
{
	class VulkanRenderPass : public RenderPass
	{
	public:
		VulkanRenderPass();
		virtual ~VulkanRenderPass();

		void SetTargetFramebuffer(std::shared_ptr<VulkanFramebuffer> targetFramebuffer) { m_TargetFramebuffer = targetFramebuffer; }

		VkRenderPass GetVkRenderPass() const { return m_VkRenderPass; }
		std::shared_ptr<VulkanFramebuffer> GetTargetFramebuffer() const { return m_TargetFramebuffer; }

	private:
		VkRenderPass m_VkRenderPass;
		std::shared_ptr<VulkanFramebuffer> m_TargetFramebuffer;

	};
}