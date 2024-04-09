#pragma once

#include "Renderer/Shader.h"

namespace NodeBrain
{
	class ComputePipeline
	{
	public:
		virtual ~ComputePipeline() = default;

		static std::shared_ptr<ComputePipeline> Create(std::shared_ptr<Shader> computeShader);
	};
}