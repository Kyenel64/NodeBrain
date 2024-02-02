#pragma once

namespace NodeBrain
{
	class RendererAPI
	{
	public:
		virtual ~RendererAPI() = default;

		virtual void Init() = 0;

		static std::unique_ptr<RendererAPI> Create();
	};
}