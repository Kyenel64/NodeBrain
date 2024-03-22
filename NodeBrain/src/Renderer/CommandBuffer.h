#pragma once

#include "Renderer/CommandPool.h"

namespace NodeBrain
{
	class CommandBuffer
	{
	public:
		virtual ~CommandBuffer() = default;
		static std::shared_ptr<CommandBuffer> Create(std::shared_ptr<CommandPool> commandPool);
	};
}