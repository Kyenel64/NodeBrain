#pragma once

#include <signal.h>

#include <spdlog/spdlog.h>

namespace NodeBrain
{
	class Log
	{
	public:
		static void Init();
		
		static std::shared_ptr<spdlog::logger>& GetLogger();
	};
}

#define NB_INFO(...)  NodeBrain::Log::GetLogger()->info(__VA_ARGS__)
#define NB_TRACE(...) NodeBrain::Log::GetLogger()->trace(__VA_ARGS__)
#define NB_WARN(...)  NodeBrain::Log::GetLogger()->warn(__VA_ARGS__)
#define NB_ERROR(...) NodeBrain::Log::GetLogger()->error(__VA_ARGS__)
#define NB_CRITICAL(...) NodeBrain::Log::GetLogger()->critical(__VA_ARGS__)

#ifdef NB_DEBUG
	#ifdef NB_WINDOWS
		#define NB_ASSERT(x, ...) { if(!(x)) { NB_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#elif NB_APPLE
		#define NB_ASSERT(x, ...) { if(!(x)) { NB_ERROR("Assertion Failed: {0}", __VA_ARGS__); raise(SIGTRAP); } }
	#elif NB_LINUX
		#define NB_ASSERT(x, ...) { if(!(x)) { NB_ERROR("Assertion Failed: {0}", __VA_ARGS__); raise(SIGTRAP); } }
	#endif
#else
	#define NB_ASSERT(x, ...)
#endif
