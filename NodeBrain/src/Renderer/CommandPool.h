#pragma once

namespace NodeBrain
{
	class CommandPool
	{
	public:
		virtual ~CommandPool() = default;
		static std::shared_ptr<CommandPool> Create();
	};
}