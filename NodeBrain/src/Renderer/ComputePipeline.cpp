#include "NBpch.h"
#include "ComputePipeline.h"

#include "Renderer/Renderer.h"
#include "GAPI/Vulkan/VulkanComputePipeline.h"
#include "GAPI/Vulkan/VulkanShader.h"

namespace NodeBrain
{
	std::shared_ptr<ComputePipeline> ComputePipeline::Create(std::shared_ptr<Shader> computeShader, std::shared_ptr<Image> targetImage)
	{
		switch (Renderer::GetGAPI())
		{
			case GAPI::None: NB_ERROR("Graphics API not detected"); return nullptr;
			case GAPI::Vulkan: return std::make_shared<VulkanComputePipeline>(std::dynamic_pointer_cast<VulkanShader>(computeShader), targetImage);
		}

		NB_ASSERT(false, "Graphics API not detected!");
		return nullptr;
	}
}