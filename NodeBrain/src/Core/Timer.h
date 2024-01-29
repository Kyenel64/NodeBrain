#pragma once

#include <chrono>

namespace NodeBrain
{
    class Timer
    {
    public:
		Timer() = default;
		~Timer() = default;

		void StartTimer() { m_StartTime = std::chrono::system_clock::now(); m_Running = true; }
		void EndTimer() { m_EndTime = std::chrono::system_clock::now(); m_Running = false; }

		float GetElapsedMilliseconds() 
		{ 
			if (!m_Running)
				return std::chrono::duration_cast<std::chrono::milliseconds>(m_EndTime - m_StartTime).count(); 
			else
				return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - m_StartTime).count();
		}
		
		float GetElapsedSeconds() { return GetElapsedMilliseconds() / 1000.0f; }

	private:
		bool m_Running = false;
		std::chrono::time_point<std::chrono::system_clock> m_StartTime;
		std::chrono::time_point<std::chrono::system_clock> m_EndTime;
	};
}