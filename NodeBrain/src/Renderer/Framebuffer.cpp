#include "NBpch.h"
#include "Framebuffer.h"

#include "Renderer/Renderer.h"
#include "GAPI/Vulkan/VulkanFramebuffer.h"
#include "GAPI/Vulkan/VulkanRenderPass.h"

namespace NodeBrain
{
	std::shared_ptr<Framebuffer> Framebuffer::Create(std::shared_ptr<RenderPass> renderPass)
	{
		switch (Renderer::GetGAPI())
		{
			case GAPI::None: NB_ERROR("Graphics API not detected"); return nullptr;
			case GAPI::Vulkan: return std::make_shared<VulkanFramebuffer>(std::dynamic_pointer_cast<VulkanRenderPass>(renderPass), nullptr); // temp
		}

		NB_ASSERT(false, "Graphics API not detected!");
		return nullptr;
	}
}