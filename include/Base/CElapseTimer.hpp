#pragma once 

#include "DevLibTypes.hpp"

namespace DevLib {

	class CElapseTimer
	{
	public:
		CElapseTimer();
		virtual ~CElapseTimer() = default;

		void Reset();

		uint32_t GetElapseTime();

		uint32_t GetElapseTimeContinue();

		void PrintElapseTime(const std::string& str = std::string("Elapsed Time"));

		void PrintElapseTimeContinue(const std::string& str = std::string("Elapsed Time"));

		static void Delay(uint32_t microsecond);

	private:
		elapse_t m_tStart;
		elapse_t m_tEnd;
		elapse_t m_frequency;

	public:
		CElapseTimer(const CElapseTimer&) = default;
		CElapseTimer(CElapseTimer&&) = default;

		CElapseTimer& operator=(const CElapseTimer&) = default;
		CElapseTimer& operator=(CElapseTimer&&) = default;
	};
}
