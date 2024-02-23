#include "NBpch.h"
#include "GraphicsPipeline.h"

#include "Renderer/Renderer.h"
#include "GAPI/Vulkan/VulkanGraphicsPipeline.h"

namespace NodeBrain
{
	std::shared_ptr<GraphicsPipeline> GraphicsPipeline::Create(std::shared_ptr<Shader> vertShader, std::shared_ptr<Shader> fragShader)
	{
		switch (Renderer::GetGAPI())
		{
			case GAPI::None: NB_ERROR("Graphics API not detected"); return nullptr;
			case GAPI::Vulkan: return std::make_unique<VulkanGraphicsPipeline>(vertShader, fragShader);
		}

		NB_ASSERT(false, "Graphics API not detected!");
		return nullptr;
	}
}