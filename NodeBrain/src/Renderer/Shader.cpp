#include "NBpch.h"
#include "Shader.h"

#include "GAPI/Vulkan/VulkanRenderContext.h"
#include "GAPI/Vulkan/VulkanShader.h"

namespace NodeBrain
{
	std::shared_ptr<Shader> Shader::Create(RenderContext& context, const std::filesystem::path& path, ShaderType shaderType)
	{
		switch (context.GetGraphicsAPI())
		{
			case GAPI::None: NB_ERROR("Graphics API not detected"); return nullptr;
			case GAPI::Vulkan: return std::make_shared<VulkanShader>(dynamic_cast<VulkanRenderContext&>(context), path, shaderType);
		}

		NB_ASSERT(false, "Graphics API not detected!");
		return nullptr;
	}
}