#pragma once

#include <chrono>

namespace NodeBrain
{
	class Timer
	{
	public:
		Timer() { StartTimer(); };
		~Timer() = default;

		void StartTimer() { m_StartTime = std::chrono::high_resolution_clock::now(); m_Running = true; }
		void EndTimer() { m_EndTime = std::chrono::high_resolution_clock::now(); m_Running = false; }

		long long GetStartTimeMicroseconds() const { return std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTime).time_since_epoch().count(); }
		long long GetStartTimeMilliseconds() const { return GetStartTimeMicroseconds() / 1000.0f; }
		long long GetStartTimeSeconds() const { return GetStartTimeMicroseconds() / 1000.0f / 1000.0f; }

		long long GetElapsedMicroseconds()
		{
			if (m_Running)
				EndTimer();

			return std::chrono::duration_cast<std::chrono::microseconds>(m_EndTime - m_StartTime).count();
		}
		long long GetElapsedMilliseconds() { return GetElapsedMicroseconds() / 1000.0f; }
		long long GetElapsedSeconds() { return GetElapsedMilliseconds() / 1000.0f / 1000.0f; }

	private:
		bool m_Running = false;
		std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTime;
		std::chrono::time_point<std::chrono::high_resolution_clock> m_EndTime;
	};
}