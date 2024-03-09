#include "NBpch.h"
#include "PipelineLayout.h"

#include "Renderer/Renderer.h"
#include "GAPI/Vulkan/VulkanPipelineLayout.h"

namespace NodeBrain
{
	std::shared_ptr<PipelineLayout> PipelineLayout::Create(const PipelineConfiguration& pipelineConfig)
	{
		switch (Renderer::GetGAPI())
		{
			case GAPI::None: NB_ERROR("Graphics API not detected"); return nullptr;
			case GAPI::Vulkan: return std::make_shared<VulkanPipelineLayout>(pipelineConfig);
		}

		NB_ASSERT(false, "Graphics API not detected!");
		return nullptr;
	}
}