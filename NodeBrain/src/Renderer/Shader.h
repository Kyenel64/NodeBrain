#pragma once

#include <filesystem>

namespace NodeBrain
{
	class Shader
	{
	public:
		virtual ~Shader() = default;

		virtual const std::filesystem::path& GetShaderPath() const = 0;

		static std::unique_ptr<Shader> Create(const std::filesystem::path& path);
	};
}