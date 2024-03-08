#pragma once

namespace NodeBrain
{
	class RenderPass
	{
	public:
		virtual ~RenderPass() = default;

		static std::shared_ptr<RenderPass> Create();
	};
}