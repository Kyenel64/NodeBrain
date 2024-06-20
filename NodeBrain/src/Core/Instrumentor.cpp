#include "NBpch.h"
#include "Instrumentor.h"

#include <fstream>
#include <thread>

namespace NodeBrain
{
	struct InstrumentorData
	{
		std::string Name = std::string();
		std::ofstream OuptutStream;
		uint32_t MaxFramesToTrace = 100;
		uint32_t FramesTraced = 0;
	};

	static InstrumentorData s_InstrumentorData;

	void Instrumentor::BeginTrace(const std::string& name, const std::filesystem::path& path)
	{
		NB_ASSERT(s_InstrumentorData.Name.empty(), "There is still an active instrumentation session");

		s_InstrumentorData.Name = name;
		s_InstrumentorData.OuptutStream.open(path);

		s_InstrumentorData.OuptutStream << "{\n \"traceEvents\": \n[\n{}";
		s_InstrumentorData.OuptutStream.flush();
	}

	void Instrumentor::EndTrace()
	{
		if (s_InstrumentorData.Name.empty())
			return;

		s_InstrumentorData.OuptutStream << "]\n}";
		s_InstrumentorData.OuptutStream.flush();

		s_InstrumentorData.OuptutStream.close();
		s_InstrumentorData.Name = std::string();
		s_InstrumentorData.FramesTraced = 0;
	}

	void Instrumentor::WriteEvent(const TraceEventData& eventData)
	{
		if (s_InstrumentorData.Name.empty())
			return;

		s_InstrumentorData.OuptutStream << ",\n{ ";
		s_InstrumentorData.OuptutStream << "\"name\": \"" << eventData.Name << "\", ";
		s_InstrumentorData.OuptutStream << "\"cat\": \"" << "Function\", ";
		s_InstrumentorData.OuptutStream << "\"ph\": \"" << "X\", ";
		s_InstrumentorData.OuptutStream << "\"ts\": " << eventData.Start << ", ";
		s_InstrumentorData.OuptutStream << "\"dur\": " << eventData.Duration << ", ";
		s_InstrumentorData.OuptutStream << "\"pid\": " << "0, ";
		s_InstrumentorData.OuptutStream << "\"tid\": " << eventData.ThreadID << " ";
		s_InstrumentorData.OuptutStream << "}";

		s_InstrumentorData.OuptutStream.flush();

		if (eventData.Name == "Frame")
			s_InstrumentorData.FramesTraced++;
		if (s_InstrumentorData.FramesTraced >= s_InstrumentorData.MaxFramesToTrace)
			EndTrace();
	}

	InstrumentorEvent::InstrumentorEvent(const std::string& name)
		: m_Data(TraceEventData())
	{
		m_Data.Name = name;
		m_Data.Start = m_Timer.GetStartTime(TimerUnit::Microseconds);
	}

	InstrumentorEvent::~InstrumentorEvent()
	{
		if (!m_Stopped)
			Stop();
	}

	void InstrumentorEvent::Stop()
	{
		m_Stopped = true;

		m_Data.Duration = m_Timer.GetElapsedTime(TimerUnit::Microseconds);
		m_Data.ThreadID = static_cast<uint32_t>(std::hash<std::thread::id>{}(std::this_thread::get_id()));
		Instrumentor::WriteEvent(m_Data);
	}

}