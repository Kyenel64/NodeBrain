#pragma once

#include <filesystem>

#include "Renderer/RenderContext.h"

namespace NodeBrain
{
	enum class ShaderType { Vertex, Fragment, Compute };

	class Shader
	{
	public:
		virtual ~Shader() = default;

		virtual const std::filesystem::path& GetShaderPath() const = 0;
		virtual ShaderType GetShaderType() const = 0;
		
		static std::shared_ptr<Shader> Create(RenderContext* context, const std::filesystem::path& path, ShaderType shaderType);
	};
}