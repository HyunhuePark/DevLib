#pragma once

#include "DevLibTypes.hpp"
#include "CVariadicCaller.hpp"

namespace DevLib {

	class CTimer
	{
	public:
		explicit CTimer(bool bUsedQueueTimer = true);
		virtual ~CTimer();

		bool StartTimer();

		template<class Func, class... Args>
		bool StartTimer(uint32_t _intervalMs, Func&& _func, Args&&... _args);

		template<class Func, class... Args>
		bool StartTimer(uint32_t _intervalMs, uint32_t _firstIntervalMs, Func&& _func, Args&&... _args);

		void StopTimer();

		template<class Func, class... Args>
		void RegisterCallBack(Func&& _func, Args&&... _args);

		void SetInterval(uint32_t _intervalMs, uint32_t _firstIntervalMs = 0);

		void SetUseQueueTimer(bool bUse);

		bool IsRunTimer() const;

		virtual void OnTimer();

	private:
		bool	m_bRunTimer;
		bool	m_bUseQueueTimer;

		uint32_t m_intervalMs;
		uint32_t m_firstIntervalMs;
		sharedHandle	m_handleTimer;
		sharedHandle	m_handleTimerQueue;
		std::unique_ptr<CAbstractCaller> m_uptrTimerCaller;

	public:
		CTimer(const CTimer&) = delete;
		CTimer(CTimer&&) = default;

		CTimer& operator=(const CTimer&) = delete;
		CTimer& operator=(CTimer&&) = default;
	};

	template<typename Func, typename... Args>
	void CTimer::RegisterCallBack(Func&& _func, Args&&... _args)
	{
		if (m_uptrTimerCaller) m_uptrTimerCaller.reset();
		m_uptrTimerCaller = std::make_unique<CVariadicCaller<Func, Args...>>(std::forward<Func>(_func), std::forward<Args>(_args)...);
	}

	template<typename Func, typename... Args>
	bool CTimer::StartTimer(const uint32_t _intervalMs, Func&& _func, Args&&... _args)
	{
		bool bRet = false;
		if (!IsRunTimer())
		{
			SetInterval(_intervalMs, _intervalMs);
			RegisterCallBack(std::forward<Func>(_func), std::forward<Args>(_args)...);
			bRet = StartTimer();
		}

		return bRet;
	}

	template<typename Func, typename... Args>
	bool CTimer::StartTimer(const uint32_t _intervalMs, const uint32_t _firstIntervalMs, Func&& _func, Args&&... _args)
	{
		bool bRet = false;
		if (!IsRunTimer())
		{
			SetInterval(_intervalMs, _firstIntervalMs);
			RegisterCallBack(std::forward<Func>(_func), std::forward<Args>(_args)...);
			bRet = StartTimer();
		}

		return bRet;
	}

}
