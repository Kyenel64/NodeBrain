#include "NBpch.h"
#include "Log.h"

#include <spdlog/sinks/stdout_color_sinks.h>

#define NB_LOG_TRACE 1

namespace NodeBrain
{
	static std::shared_ptr<spdlog::logger> s_Logger;

	void Log::Init()
	{
		NB_PROFILE_FN();

		s_Logger = spdlog::stdout_color_mt("NodeBrain");

		#if NB_LOG_TRACE
			s_Logger->set_pattern("[%T] [%n] [%s:%#]: %v %$");
		#else
			s_Logger->set_pattern("[%T] [%n]: %v %$");
		#endif
	}

	std::shared_ptr<spdlog::logger>& Log::GetLogger() { return s_Logger; }
}