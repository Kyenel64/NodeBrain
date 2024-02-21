#pragma once

#include <filesystem>

#include "Core/Timer.h"

#define NB_ENABLE_PROFILING

namespace NodeBrain
{
	struct TraceEventData
	{
		std::string Name;
		long long Start;
		long long Duration;
		uint32_t ThreadID;
	};

	class Instrumentor
	{
	public:
		static void BeginTrace(const std::string& name = "Trace", const std::filesystem::path& path = "Trace.log");
		static void EndTrace();

		static void WriteEvent(const TraceEventData& eventData);
	};

	class InstrumentorEvent
	{
	public:
		InstrumentorEvent(const std::string& name);
		~InstrumentorEvent();

		void Stop();

	private:
		TraceEventData m_Data;
		Timer m_Timer;
		bool m_Stopped = false;
	};

#ifdef NB_ENABLE_PROFILING
	#define NB_PROFILE_BEGIN(name, path) ::NodeBrain::Instrumentor::BeginTrace(name, path)
	#define NB_PROFILE_END() ::NodeBrain::Instrumentor::EndTrace()
	#define NB_PROFILE_SCOPE(name) ::NodeBrain::InstrumentorEvent profileEvent(name)
	#define NB_PROFILE_FN() ::NodeBrain::InstrumentorEvent(__FUNCSIG__)
#else
	#define NB_PROFILE_BEGIN(name, path) 
	#define NB_PROFILE_END()
	#define NB_PROFILE_SCOPE(name)
	#define NB_PROFILE_FN()
#endif
}