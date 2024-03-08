#pragma once

#include "Renderer/PipelineLayout.h"
#include "Renderer/RenderPass.h"

namespace NodeBrain
{
	class GraphicsPipeline
	{
	public:
		virtual ~GraphicsPipeline() = default;
		static std::shared_ptr<GraphicsPipeline> Create(std::shared_ptr<PipelineLayout> layout, std::shared_ptr<RenderPass> renderPass);
	};
}