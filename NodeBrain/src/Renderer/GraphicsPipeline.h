#pragma once

#include "Renderer/Shader.h"

namespace NodeBrain
{
	class GraphicsPipeline
	{
	public:
		virtual ~GraphicsPipeline() = default;

		static std::shared_ptr<GraphicsPipeline> Create(std::shared_ptr<Shader> vertShader, std::shared_ptr<Shader> fragShader);
	};
}