#include "NBpch.h"
#include "Log.h"

#include <spdlog/sinks/stdout_color_sinks.h>

namespace NodeBrain
{
	static std::shared_ptr<spdlog::logger> s_Logger;

	void Log::Init()
	{
		NB_PROFILE_FN();

		spdlog::set_pattern("[%T] [%n]: %v %$");
		s_Logger = spdlog::stdout_color_mt("NodeBrain");
	}

	std::shared_ptr<spdlog::logger>& Log::GetLogger() { return s_Logger; }
}