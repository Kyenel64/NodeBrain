#pragma once

#include <chrono>

namespace NodeBrain
{
	enum class TimerUnit { Seconds, Milliseconds, Microseconds };

	class Timer
	{
	public:
		Timer() { Reset(); };
		~Timer() = default;

		void Reset() { m_StartTime = std::chrono::high_resolution_clock::now(); }

		[[nodiscard]] double GetElapsedTime(TimerUnit unit)
		{
			switch (unit)
			{
				case TimerUnit::Seconds:
				{
					auto elapsedTime = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - m_StartTime);
					return static_cast<double>(elapsedTime.count());
				}
				case TimerUnit::Milliseconds:
				{
					auto elapsedTime = std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - m_StartTime);
					return static_cast<double>(elapsedTime.count());
				}
				case TimerUnit::Microseconds:
				{
					auto elapsedTime = std::chrono::duration<double, std::micro>(std::chrono::high_resolution_clock::now() - m_StartTime);
					return static_cast<double>(elapsedTime.count());
				}
			}

			NB_ASSERT(false, "Invalid TimerUnit. unit must be a valid TimerUnit value.");
			return 0.0f;
		}

		[[nodiscard]] long long GetStartTime(TimerUnit unit)
		{
			switch (unit)
			{
				case TimerUnit::Seconds:
				{
					auto elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(m_StartTime.time_since_epoch());
					return static_cast<long long>(elapsedTime.count());
				}
				case TimerUnit::Milliseconds:
				{
					auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(m_StartTime.time_since_epoch());
					return static_cast<long long>(elapsedTime.count());
				}
				case TimerUnit::Microseconds:
				{
					auto elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(m_StartTime.time_since_epoch());
					return static_cast<long long>(elapsedTime.count());
				}
			}

			NB_ASSERT(false, "Invalid TimerUnit. unit must be a valid TimerUnit value.");
			return 0;
		}

	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTime;
	};
}