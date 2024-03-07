#pragma once

#include "Renderer/PipelineConfiguration.h"

namespace NodeBrain
{
    class PipelineLayout
    {
    public:
        virtual ~PipelineLayout() = default;
		static std::shared_ptr<PipelineLayout> Create(const PipelineConfiguration& pipelineConfig);
    };
}