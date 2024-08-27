#pragma once

#include <filesystem>

#include "Renderer/RenderContext.h"
#include "Renderer/DescriptorSet.h"

namespace NodeBrain
{
	enum class ShaderType { Vertex, Fragment, Compute };

	class Shader
	{
	public:
		virtual ~Shader() = default;

		[[nodiscard]] virtual const std::filesystem::path& GetShaderPath() const = 0;
		[[nodiscard]] virtual ShaderType GetShaderType() const = 0;
		[[nodiscard]] virtual std::vector<LayoutBinding> GetLayout() const = 0;
		
		static std::shared_ptr<Shader> Create(RenderContext& context, const std::filesystem::path& path, ShaderType shaderType);
	};
}
