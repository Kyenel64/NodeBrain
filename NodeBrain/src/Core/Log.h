#pragma once

#include <csignal>
#include <format>

#include <spdlog/spdlog.h>

namespace NodeBrain::Log
{
	std::shared_ptr<spdlog::logger>& GetLogger();
}

#define NB_INFO(...)     SPDLOG_LOGGER_INFO(NodeBrain::Log::GetLogger(), __VA_ARGS__)
#define NB_TRACE(...)    SPDLOG_LOGGER_TRACE(NodeBrain::Log::GetLogger(), __VA_ARGS__)
#define NB_WARN(...)     SPDLOG_LOGGER_WARN(NodeBrain::Log::GetLogger(), __VA_ARGS__)
#define NB_ERROR(...)    SPDLOG_LOGGER_ERROR(NodeBrain::Log::GetLogger(), __VA_ARGS__)
#define NB_CRITICAL(...) SPDLOG_LOGGER_CRITICAL(NodeBrain::Log::GetLogger(), __VA_ARGS__)

#ifdef NB_DEBUG
	#ifdef NB_WINDOWS
		#define NB_ASSERT(x, ...) { if(!(x)) { NB_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#elif NB_APPLE
		#define NB_ASSERT(x, ...) { if(!(x)) { NB_ERROR("Assertion Failed: {0}", std::format(__VA_ARGS__)); raise(SIGTRAP); } }
	#elif NB_LINUX
		#define NB_ASSERT(x, ...) { if(!(x)) { NB_ERROR("Assertion Failed: {0}", __VA_ARGS__); raise(SIGTRAP); } }
	#endif
#else
	#define NB_ASSERT(x, ...)
#endif
