#include "NBpch.h"
#include "Shader.h"

#include "Renderer/Renderer.h"
#include "GAPI/Vulkan/VulkanShader.h"

namespace NodeBrain
{
	std::unique_ptr<Shader> Shader::Create(const std::filesystem::path& path, ShaderType shaderType)
	{
		switch (Renderer::GetGAPI())
		{
			case GAPI::None: NB_ERROR("Graphics API not detected"); return nullptr;
			case GAPI::Vulkan: return std::make_unique<VulkanShader>(path, shaderType);
		}

		NB_ASSERT(false, "Graphics API not detected!");
		return nullptr;
	}
}