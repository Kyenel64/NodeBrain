#pragma once

#include "Renderer/Shader.h"

namespace NodeBrain
{
	class ComputePipeline
	{
	public:
		virtual ~ComputePipeline() = default;

		virtual void SetPushConstantData(const void* buffer, uint32_t size, uint32_t offset) = 0;

		static std::shared_ptr<ComputePipeline> Create(std::shared_ptr<Shader> computeShader);
	};
}