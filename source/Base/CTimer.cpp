#include "../../include/Base/CTimer.hpp"

#if defined(_MSC_VER)
#pragma warning(disable : 5105)
#include <Windows.h>
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "ntdll.lib")
#pragma warning(default : 5105)

extern "C" NTSYSAPI NTSTATUS NTAPI NtSetTimerResolution(ULONG DesiredResolution, BOOLEAN SetResolution, PULONG CurrentResolution);
#elif defined(__linux__) // Linux
#include <csignal>
#include <cstring>
#endif

namespace DevLib {

#if defined(_MSC_VER)
	void __stdcall innerQueueTimerCall(PVOID func, BOOLEAN)
	{
		CTimer* pThis = nullptr;
		memcpy(&pThis, &func, sizeof(void*));

		if (pThis) pThis->OnTimer();
	}

	void __stdcall innerMMTimerCall(UINT, UINT, const DWORD_PTR dwUser, DWORD_PTR, DWORD_PTR)
	{
		CTimer* pThis = nullptr;
		memcpy(&pThis, &dwUser, sizeof(DWORD_PTR));

		if (pThis) pThis->OnTimer();
	}

#elif defined(__linux__) // Linux

	static void innerTimerCaller(sigval_t sv)
	{
		CTimer* pThis = nullptr;
		memcpy(&pThis, &sv.sival_ptr, sizeof(sv.sival_ptr));

		if (pThis) pThis->OnTimer();
	}

#endif

	CTimer::CTimer(const bool bUsedQueueTimer /*= true*/)
		: m_bRunTimer(false),
		m_bUseQueueTimer(bUsedQueueTimer),
		m_intervalMs(0),
		m_firstIntervalMs(0), m_handleTimer(std::make_shared<void*>(nullptr)),
		m_handleTimerQueue(std::make_shared<void*>(nullptr))
	{

	}

	CTimer::~CTimer()
	{
		if (m_handleTimer.use_count() == 1)
		{
			StopTimer();
		}
	}

	void CTimer::SetInterval(uint32_t _intervalMs, uint32_t _firstIntervalMs /*= 0*/)
	{
		if (_firstIntervalMs > 0) m_firstIntervalMs = _firstIntervalMs;
		m_intervalMs = _intervalMs;
	}

	void CTimer::SetUseQueueTimer(const bool bUse)
	{
		if (m_bUseQueueTimer != bUse)
		{
			if (IsRunTimer())
			{
				StopTimer();
				StartTimer();
			}
		}

		m_bUseQueueTimer = bUse;
	}

	bool CTimer::StartTimer()
	{

		if (m_bRunTimer) StopTimer();

#if defined(_MSC_VER)
		ULONG cur = 0;

		// Time Resolution 
		(void)NtSetTimerResolution(5000, TRUE, &cur);
		//timeBeginPeriod(1);

		if (m_bUseQueueTimer)
		{
			*m_handleTimerQueue = CreateTimerQueue();
			m_bRunTimer = CreateTimerQueueTimer(m_handleTimer.get(), *m_handleTimerQueue, innerQueueTimerCall, this,
				m_firstIntervalMs, m_intervalMs, WT_EXECUTEDEFAULT);
		}
		else
		{
			const MMRESULT hTimer = timeSetEvent(m_intervalMs, 1, innerMMTimerCall, reinterpret_cast<DWORD_PTR>(this), TIME_PERIODIC);
			memcpy(&*m_handleTimer, &hTimer, sizeof(MMRESULT));

			if (*m_handleTimer) m_bRunTimer = true;
			else m_bRunTimer = false;
		}

#elif defined(__linux__) // Linux
		sigevent ptimerEvent;
		ptimerEvent.sigev_value.sival_ptr = this;							// @suppress("Field cannot be resolved")
		ptimerEvent.sigev_notify = SIGEV_THREAD; 							// @suppress("Field cannot be resolved")
		ptimerEvent.sigev_notify_function = &innerTimerCaller;				// @suppress("Field cannot be resolved")
		ptimerEvent.sigev_notify_attributes = nullptr; 						// @suppress("Field cannot be resolved")

		if (timer_create(CLOCK_REALTIME, &ptimerEvent, m_handleTimer.get()) == 0)
		{
			const int64_t sec = m_intervalMs / 1000;
			const int64_t msec = (m_intervalMs % 1000) * 1000000;

			int64_t first_sec = m_firstIntervalMs / 1000;
			int64_t first_msec = (m_firstIntervalMs % 1000) * 1000000;

			if( first_sec == 0 && first_msec == 0 )
			{
				first_sec = sec;
				first_msec = msec;
			}
			
			const struct itimerspec _Time = { { sec, msec},{ first_sec, first_msec} };

			if (timer_settime(*m_handleTimer, 0, &_Time, NULL) == 0)
			{
				m_bRunTimer = true;
			}

		}
#endif
		return m_bRunTimer;
	}

	void CTimer::StopTimer()
	{
		if (m_bRunTimer)
		{
			m_bRunTimer = false;

			if (*m_handleTimer)
			{
#if defined(_MSC_VER)
				(void)DeleteTimerQueueTimer(*m_handleTimerQueue, *m_handleTimer, nullptr);
#elif defined(__linux__) // Linux
				(void)timer_delete(*m_handleTimer);
#endif
			}
			else
			{
#if defined(_MSC_VER)
#if defined(_WIN64) 
				const auto pData = reinterpret_cast<uint64_t>(*m_handleTimer);
				timeKillEvent(static_cast<UINT>(pData));
#else // Windows x86
				timeKillEvent(reinterpret_cast<UINT>(*m_handleTimer));
#endif
#elif defined(__linux__) // Linux

#endif
			}

			*m_handleTimer = nullptr;
		}
	}

	bool CTimer::IsRunTimer() const
	{
		return m_bRunTimer;
	}

	void CTimer::OnTimer()
	{
		if (m_uptrTimerCaller) m_uptrTimerCaller->call();
	}

}
