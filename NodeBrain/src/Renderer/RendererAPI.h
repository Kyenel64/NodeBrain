#pragma once

namespace NodeBrain
{
	class RendererAPI
	{
	public:
		virtual ~RendererAPI() = default;

		static std::unique_ptr<RendererAPI> Create();
	};
}