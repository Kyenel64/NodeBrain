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
		uint32_t MaxFramesToTrace = 1000;
		uint32_t FramesTraced = 0;

		std::stack<std::string> EventDatas;
		std::mutex EventDatasMutex;
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

		while (!s_InstrumentorData.EventDatas.empty())
		{
			s_InstrumentorData.OuptutStream << s_InstrumentorData.EventDatas.top();
			s_InstrumentorData.EventDatas.pop();
		}

		s_InstrumentorData.OuptutStream << "]\n}";
		s_InstrumentorData.OuptutStream.flush();

		s_InstrumentorData.OuptutStream.close();
		s_InstrumentorData.Name = std::string();
		s_InstrumentorData.FramesTraced = 0;
	}

	void Instrumentor::PushEventData(const std::string& data)
	{
		std::lock_guard<std::mutex> lock(s_InstrumentorData.EventDatasMutex);
		s_InstrumentorData.EventDatas.push(data);
	}




	InstrumentorEvent::InstrumentorEvent(const std::string& name)
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

		std::stringstream data;
		data << ",\n{ ";
		data << "\"name\": \"" << m_Data.Name << "\", ";
		data << "\"cat\": \"" << "Function\", ";
		data << "\"ph\": \"" << "X\", ";
		data << "\"ts\": " << m_Data.Start << ", ";
		data << "\"dur\": " << m_Data.Duration << ", ";
		data << "\"pid\": " << "0, ";
		data << "\"tid\": " << m_Data.ThreadID << " ";
		data << "}";

		Instrumentor::PushEventData(data.str());
	}

}