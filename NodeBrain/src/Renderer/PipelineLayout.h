#pragma once

#include "Renderer/PipelineData.h"

namespace NodeBrain
{
    class PipelineLayout
    {
    public:
        virtual ~PipelineLayout() = default;
		static std::shared_ptr<PipelineLayout> Create(const PipelineData& pipelineState);
    };
}