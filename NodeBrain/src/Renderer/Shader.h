#pragma once

#include <filesystem>

#include "Renderer/RenderContext.h"
#include "Renderer/DescriptorSet.h"

namespace NodeBrain
{
	enum class ShaderType { Vertex, Fragment, Compute };

	enum class InputFormat { None = 0, R32, R32G32, R32G32B32, R32G32B32A32 };

	struct InputVariable
	{
		std::string Name;
		uint32_t Location;
		InputFormat Format;
		uint32_t Offset;
	};

	class Shader
	{
	public:
		virtual ~Shader() = default;

		[[nodiscard]] virtual const std::filesystem::path& GetShaderPath() const = 0;
		[[nodiscard]] virtual ShaderType GetShaderType() const = 0;
		[[nodiscard]] virtual std::vector<LayoutBinding> GetLayout() const = 0;
		[[nodiscard]] virtual std::vector<InputVariable> GetInputVariables() const = 0;
		
		static std::shared_ptr<Shader> Create(RenderContext& context, const std::filesystem::path& path);
	};
}
