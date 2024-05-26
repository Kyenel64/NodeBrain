#include "NBpch.h"
#include "Log.h"

#include <spdlog/sinks/stdout_color_sinks.h>

namespace NodeBrain
{
	namespace Log
	{
		static std::shared_ptr<spdlog::logger> s_Logger;

		std::shared_ptr<spdlog::logger>& GetLogger()
		{
			// Initialize if not yet initialized
			if (!s_Logger)
			{
				s_Logger = spdlog::stdout_color_mt("NodeBrain");
				s_Logger->set_pattern("[%T] [%n] [%s:%#]: %v %$");
			}

			return s_Logger;
		}
	}
}