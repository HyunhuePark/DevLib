#include "../../include/Base/CTime.hpp"

#include <ctime>
#include <sys/timeb.h>
#include <cstdio>
#include <cmath>
#include <iomanip>
#include <cstring>

#if defined(_MSC_VER)
#elif defined(__linux__) // Linux 
#define sprintf_s(buf, ...) snprintf((buf), sizeof(buf), __VA_ARGS__)
#endif


namespace DevLib
{

	CTime::CTime()
	{
		m_time.tm_sec = 0;
		m_time.tm_min = 0;
		m_time.tm_hour = 0;
		m_time.tm_mday = 0;
		m_time.tm_mon = 0;
		m_time.tm_year = 0;
		m_time.tm_wday = 0;
		m_time.tm_yday = 0;
		m_time.tm_isdst = 0;
		m_time.tm_ms = 0;
		m_time.tm_us = 0;

		UpdateCurrentTime();
	}

	CTime::CTime(const double& timestamp)
	{
		const auto second = static_cast<time_t>(timestamp);
		const auto msecond = static_cast<int32_t>((timestamp - static_cast<double_t>(second)) * 1000);
		const auto usecond = static_cast<int32_t>((timestamp - static_cast<double_t>(second)) * 1000000);

		tm temp;
#if defined(_MSC_VER)
		(void)localtime_s(reinterpret_cast<tm*>(&temp), &second);
#elif defined(__linux__) // Linux
		localtime_r(&second, reinterpret_cast<tm*>(&temp));
		
#endif
		m_time.tm_sec = temp.tm_sec;
		m_time.tm_min = temp.tm_min;
		m_time.tm_hour = temp.tm_hour;
		m_time.tm_mday = temp.tm_mday;
		m_time.tm_mon = temp.tm_mon;
		m_time.tm_year = temp.tm_year;
		m_time.tm_wday = temp.tm_wday;
		m_time.tm_yday = temp.tm_yday;
		m_time.tm_isdst = temp.tm_isdst;

		m_time.tm_ms = msecond;
		m_time.tm_us = usecond;
		m_time.time_t = second;
		m_time.tm_year += 1900;
		m_time.tm_mon += 1;
	}

	CTime::~CTime() = default;

	void CTime::UpdateCurrentTime()
	{
		tm temp;
#if defined(_MSC_VER)
		__timeb64 tb{};
		_ftime_s(&tb);
		(void)localtime_s(reinterpret_cast<tm*>(&temp), &tb.time);

		m_time.tm_ms = tb.millitm;
		m_time.tm_us = tb.millitm * 1000;
		m_time.time_t = static_cast<long>(tb.time);
#elif defined(__linux__) // Linux
		timespec curTime{};
		clock_gettime(CLOCK_REALTIME, &curTime);

		localtime_r(&curTime.tv_sec, reinterpret_cast<tm*>(&temp));
		m_time.time_t = curTime.tv_sec;
		m_time.tm_ms = static_cast<int32_t>(curTime.tv_nsec * 0.000001);
		m_time.tm_us = static_cast<int32_t>(curTime.tv_nsec * 0.001);
#endif

		m_time.tm_sec = temp.tm_sec;
		m_time.tm_min = temp.tm_min;
		m_time.tm_hour = temp.tm_hour;
		m_time.tm_mday = temp.tm_mday;
		m_time.tm_mon = temp.tm_mon;
		m_time.tm_year = temp.tm_year;
		m_time.tm_wday = temp.tm_wday;
		m_time.tm_yday = temp.tm_yday;
		m_time.tm_isdst = temp.tm_isdst;

		m_time.tm_year += 1900;
		m_time.tm_mon += 1;

	}

	void CTime::PrintTime() const
	{
		printf("%04d-%02d-%02d_%02d-%02d-%02d-%03d\n", m_time.tm_year, m_time.tm_mon, m_time.tm_mday, m_time.tm_hour, m_time.tm_min, m_time.tm_sec, m_time.tm_ms);
	}

	void CTime::PrintTimeToString(std::string& str) const
	{
		char buff[1024] = "";
		(void)sprintf_s(buff, "%04d-%02d-%02d_%02d-%02d-%02d-%03d", m_time.tm_year, m_time.tm_mon, m_time.tm_mday, m_time.tm_hour, m_time.tm_min, m_time.tm_sec, m_time.tm_ms);

		str = std::string(buff);
	}

	std::string CTime::GetTimeToString() const
	{
		char buff[1024] = "";
		(void)sprintf_s(buff, "%04d-%02d-%02d_%02d-%02d-%02d-%03d", m_time.tm_year, m_time.tm_mon, m_time.tm_mday, m_time.tm_hour, m_time.tm_min, m_time.tm_sec, m_time.tm_ms);

		return { buff };
	}

    double CTime::GetTimestamp() const
    {
        // return mktime(reinterpret_cast<tm*>(&m_time)) + m_time.tm_ms * 0.001;
		return static_cast<double>(m_time.time_t + m_time.tm_ms * 0.001);
    }

    CTime CTime::operator-(const CTime &a) const
    {
		CTime ret;

		memset(&ret.m_time, 0, sizeof ret.m_time);
		const uint64_t elapse = abs(static_cast<int64_t>(m_time.time_t * 1000 + m_time.tm_ms - (a.m_time.time_t * 1000 + a.m_time.tm_ms)));
		const uint64_t msElapse = elapse % 1000;

		const time_t t = abs(static_cast<int64_t>(m_time.time_t - a.m_time.time_t));
#if defined(_MSC_VER)
		(void)gmtime_s(reinterpret_cast<tm*>(&ret.m_time), &t);
#elif defined(__linux__) // Linux
		gmtime_r(&t, reinterpret_cast<tm*>(&ret.m_time));
#endif
		ret.m_time.tm_year -= 70;
		ret.m_time.tm_mday -= 1;

		ret.m_time.time_t = static_cast<int64_t>(t);
		ret.m_time.tm_ms = static_cast<int32_t>(msElapse);

		return ret;
	}

	CTime CTime::operator-(const uint64_t sec) const
	{
		CTime ret;

		const time_t t = abs(static_cast<int64_t>(m_time.time_t - sec));
#if defined(_MSC_VER)
		(void)gmtime_s(reinterpret_cast<tm*>(&ret.m_time), &t);
#elif defined(__linux__) // Linux
		gmtime_r(&t, reinterpret_cast<tm*>(&ret.m_time));
#endif

		ret.m_time.tm_year -= 70;
		ret.m_time.tm_mon -= 1;
		ret.m_time.time_t = t;
		ret.m_time.tm_ms = m_time.tm_ms;

		return ret;
	}

	CTime CTime::operator+(const uint64_t sec) const
	{
		CTime ret;

		const time_t t = abs(static_cast<int64_t>(m_time.time_t + sec));
#if defined(_MSC_VER)
		(void)gmtime_s(reinterpret_cast<tm*>(&ret.m_time), &t);
#elif defined(__linux__) // Linux
		gmtime_r(&t, reinterpret_cast<tm*>(&ret.m_time));
#endif

		ret.m_time.tm_year += 1900;
		ret.m_time.tm_mon += 1;
		ret.m_time.time_t = t;
		ret.m_time.tm_ms = m_time.tm_ms;

		return ret;
	}
	
	CTime CTime::MakeCurrentTime()
	{
		CTime time;
		time.UpdateCurrentTime();
		return time;
	}

	double CTime::MakeCurrentTimestamp()
	{
		CTime time;
		time.UpdateCurrentTime();
		return time.GetTimestamp();
	}


}
