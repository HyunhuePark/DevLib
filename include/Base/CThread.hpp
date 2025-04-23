#pragma once

#include "DevLibTypes.hpp"
#include "CVariadicCaller.hpp"
#include <list>

namespace DevLib {

	class CThread
	{
	public:
		explicit CThread();
		virtual ~CThread();

		template<typename Func, typename... Args>
		bool StartThread(Func&& _func, Args&&... _args);

		void StopThread();

		bool IsRunThread() const;

		bool WaitForEndThread(uint32_t waitMillisecond = 0xFFFFFFFF);

		virtual void OnThread();

		uint32_t GetID() const { return m_threadID; }
		sharedHandle GetHandle() const { return m_threadHandle; }
		
		static void microSleep(size_t waitMicro);

	private:
		bool BeginThread();

		bool m_bRunThread;
		uint32_t m_threadID;
		sharedHandle m_threadHandle;
		std::unique_ptr<CAbstractCaller> m_uptrThreadCaller;

	public:
		CThread(const CThread&) = delete;
		CThread(CThread&&) = default;

		CThread& operator=(const CThread&) = delete;
		CThread& operator=(CThread&&) = default;

		using listThread = std::list<CThread*>;

		static CThread* GetCurrentThread();
		static listThread GetThreadList();
	};

	template<typename Func, typename... Args>
	bool CThread::StartThread(Func&& _func, Args&&... _args)
	{
		bool bRet = false;
		if (!IsRunThread())
		{
			if (m_uptrThreadCaller) m_uptrThreadCaller.reset();
			m_uptrThreadCaller = std::make_unique<CVariadicCaller<Func, Args...>>(std::forward<Func>(_func), std::forward<Args>(_args)...);
			bRet = BeginThread();
		}

		return bRet;
	}
}
