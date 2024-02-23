#pragma once

namespace NodeBrain
{
	class Shader
	{
	public:
		virtual ~Shader() = default;

		static std::unique_ptr<Shader> Create(const std::filesystem::path& path);
	};
}