#include "NBpch.h"
#include "Framebuffer.h"

#include "Renderer/Renderer.h"
#include "GAPI/Vulkan/VulkanFramebuffer.h"

namespace NodeBrain
{
	std::shared_ptr<Framebuffer> Framebuffer::Create(const FramebufferConfiguration& configuration)
	{
		switch (Renderer::GetGAPI())
		{
			case GAPI::None: NB_ERROR("Graphics API not detected"); return nullptr;
			case GAPI::Vulkan: return std::make_shared<VulkanFramebuffer>(configuration);
		}

		NB_ASSERT(false, "Graphics API not detected!");
		return nullptr;
	}
}