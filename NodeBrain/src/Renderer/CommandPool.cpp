#include "NBpch.h"
#include "CommandPool.h"

#include "Renderer/Renderer.h"
#include "GAPI/Vulkan/VulkanCommandPool.h"

namespace NodeBrain
{
	std::shared_ptr<CommandPool> CommandPool::Create()
	{
		switch (Renderer::GetGAPI())
		{
			case GAPI::None: NB_ERROR("Graphics API not detected"); return nullptr;
			case GAPI::Vulkan: return std::make_shared<VulkanCommandPool>();
		}

		NB_ASSERT(false, "Graphics API not detected!");
		return nullptr;
	}
}