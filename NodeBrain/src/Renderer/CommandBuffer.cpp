#include "NBpch.h"
#include "CommandBuffer.h"

#include "Renderer/Renderer.h"
#include "GAPI/Vulkan/VulkanCommandBuffer.h"

namespace NodeBrain
{
	std::shared_ptr<CommandBuffer> CommandBuffer::Create(std::shared_ptr<CommandPool> commandPool)
	{
		switch (Renderer::GetGAPI())
		{
			case GAPI::None: NB_ERROR("Graphics API not detected"); return nullptr;
			case GAPI::Vulkan: return std::make_shared<VulkanCommandBuffer>(std::dynamic_pointer_cast<VulkanCommandPool>(commandPool));
		}

		NB_ASSERT(false, "Graphics API not detected!");
		return nullptr;
	}
}