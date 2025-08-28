#include "../../include/Base/CElapseTimer.hpp"

#include <cstdio>
#if defined(_MSC_VER)
#pragma warning(disable : 5105)
#include <Windows.h>
#pragma warning(default : 5105)
#elif defined(__linux__) // Linux
#include <unistd.h>
#endif

namespace DevLib {

	CElapseTimer::CElapseTimer()
#if defined(_MSC_VER)
		: m_tStart(0), m_tEnd(0), m_frequency(0)
	{
		SetThreadAffinityMask(GetCurrentThread(), 0);
		QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&m_frequency));

#elif defined(__linux__) // Linux
		{

#endif
			Reset();
		}

	void CElapseTimer::Reset()
	{
#if defined(_MSC_VER)
		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&m_tStart));

#elif defined(__linux__) // Linux
		clock_gettime(CLOCK_REALTIME, &m_tStart);
#endif

	}

	uint32_t CElapseTimer::GetElapseTime()
	{
#if defined(_MSC_VER)
		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&m_tEnd));

		uint64_t* s = &m_tStart;
		const uint64_t* e = &m_tEnd;
		const uint64_t* f = &m_frequency;

		const auto elapsed = static_cast<uint32_t>(static_cast<double>(*e - *s) / static_cast<double>(*f) * 1000000.0);
		*s = *e;
#elif defined(__linux__) // Linux
		clock_gettime(CLOCK_REALTIME, &m_tEnd);

		const uint64_t etime = m_tEnd.tv_sec * 1000000 + static_cast<uint64_t>(static_cast<float_t>(m_tEnd.tv_nsec) * 0.001F);
		const uint64_t stime = m_tStart.tv_sec * 1000000 + static_cast<uint64_t>(static_cast<float_t>(m_tStart.tv_nsec) * 0.001F);

		const uint32_t elapsed = static_cast<uint32_t>(etime - stime);
		m_tStart = m_tEnd;

#endif

		return elapsed;
	}

	uint32_t CElapseTimer::GetElapseTimeContinue()
	{
#if defined(_MSC_VER)
		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&m_tEnd));
		const uint64_t* s = &m_tStart;
		const uint64_t* e = &m_tEnd;
		const uint64_t* f = &m_frequency;

		const auto elapsed = static_cast<uint32_t>(static_cast<double>(*e - *s) / static_cast<double>(*f) * 1000000.0);
#elif defined(__linux__) // Linux
		clock_gettime(CLOCK_REALTIME, &m_tEnd);

		const uint64_t etime = m_tEnd.tv_sec * 1000000 + m_tEnd.tv_nsec * 0.001;
		const uint64_t stime = m_tStart.tv_sec * 1000000 + m_tStart.tv_nsec * 0.001;

		const auto elapsed = static_cast<uint32_t>(etime - stime);
#endif

		return elapsed;
	}

	void CElapseTimer::PrintElapseTime(const std::string & str /*= std::string("Elapsed Time")*/)
	{
		printf("%s : %lf sec\n", str.c_str(), GetElapseTime() * 0.000001);
	}

	void CElapseTimer::PrintElapseTimeContinue(const std::string & str /*= std::string("Elapsed Time")*/)
	{
		printf("%s : %lf sec\n", str.c_str(), GetElapseTimeContinue() * 0.000001);
	}

	void CElapseTimer::Delay(uint32_t microsecond)
	{
#if defined(_MSC_VER)
		static LARGE_INTEGER BeginTime;
		static LARGE_INTEGER EndTime;
		static LARGE_INTEGER Frequency;
		QueryPerformanceCounter(&BeginTime);

		QueryPerformanceFrequency(&Frequency);

		while (true)
		{
			QueryPerformanceCounter(&EndTime);
			if ((static_cast<float>(EndTime.QuadPart - BeginTime.QuadPart) / static_cast<float>(Frequency.QuadPart)) * 1000000.0f >= static_cast<float>(microsecond))
				break;
		}
#elif defined(__linux__) // Linux
		usleep(microsecond);
#endif
	}
	}
