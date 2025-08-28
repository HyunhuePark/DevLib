#pragma once 

#include "DevLibTypes.hpp"

namespace DevLib {

	class CTime
	{
	public:
		CTime();
		CTime(const double& timestamp);
		virtual ~CTime();

		void UpdateCurrentTime();

		int32_t Year() const { return m_time.tm_year; }

		int32_t Month() const { return m_time.tm_mon; }

		int32_t Day() const { return m_time.tm_mday; }

		int32_t Hour() const { return m_time.tm_hour; }

		int32_t Min() const { return m_time.tm_min; }

		int32_t Second() const { return m_time.tm_sec; }

		int32_t MilliSecond() const { return m_time.tm_ms; }

		int32_t MicroSecond() const { return m_time.tm_us; }

		int32_t YearDay() const { return m_time.tm_yday; }

		void PrintTime() const;
		void PrintTimeToString(std::string& str) const;

		std::string GetTimeToString() const;
		double GetTimestamp() const;

		CTime operator-(const CTime& a) const;
		CTime operator-(uint64_t sec) const;
		CTime operator+(uint64_t sec) const;

		static CTime MakeCurrentTime();
		static double MakeCurrentTimestamp();

		struct sTIME {
			int32_t tm_sec;     /* seconds after the minute - [0,59] */
			int32_t tm_min;     /* minutes after the hour - [0,59] */
			int32_t tm_hour;    /* hours since midnight - [0,23] */
			int32_t tm_mday;    /* day of the month - [1,31] */
			int32_t tm_mon;     /* months since January - [1,12] */
			int32_t tm_year;    /* Year [xxxx] */
			int32_t tm_wday;    /* days since Sunday - [0,6] */
			int32_t tm_yday;    /* days since January 1 - [0,365] */
			int32_t tm_isdst;   /* daylight savings time flag */
			int32_t tm_ms;
			int32_t tm_us;
			uint64_t time_t;
		} m_time{};

		CTime(const CTime&) = default;
		CTime(CTime&&) = default;

		CTime& operator=(const CTime&) = default;
		CTime& operator=(CTime&&) = default;
	};
}
