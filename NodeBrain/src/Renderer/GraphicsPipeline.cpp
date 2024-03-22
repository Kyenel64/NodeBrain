#include "NBpch.h"
#include "GraphicsPipeline.h"

#include "Renderer/Renderer.h"
#include "GAPI/Vulkan/VulkanGraphicsPipeline.h"

namespace NodeBrain
{
	std::shared_ptr<GraphicsPipeline> GraphicsPipeline::Create(const PipelineConfiguration& configuration)
	{
		switch (Renderer::GetGAPI())
		{
			case GAPI::None: NB_ERROR("Graphics API not detected"); return nullptr;
			case GAPI::Vulkan: return std::make_shared<VulkanGraphicsPipeline>(configuration);
		}

		NB_ASSERT(false, "Graphics API not detected!");
		return nullptr;
	}
}