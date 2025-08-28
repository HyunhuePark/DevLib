#include "../../include/Base/CThread.hpp"
#include "../../include/Base/CLocker.hpp"

#if defined(_MSC_VER)
#pragma warning(disable : 5105)
#include <Windows.h>
#include <process.h>
#include <xthreads.h>
#pragma warning(default : 5105)
#elif defined(__linux__) // Linux
#include <pthread.h>
#include <thread>
#include <unistd.h>
#endif

#include <list>

namespace DevLib
{
	std::once_flag _devlib_thread_onceFlag;
	std::unique_ptr<CLocker<CThread::listThread>> _devlib_manage_thread_;

	CThread* CThread::GetCurrentThread()
	{
		CThread* pRes = nullptr;

#if defined(_MSC_VER)
		const uint32_t _thread_id = _Thrd_id();
#elif defined(__linux__) // Linux
		const uint32_t _thread_id = pthread_self();
#endif
		_devlib_manage_thread_->Lock();

		for (CThread* pThread : _devlib_manage_thread_->get())
		{
			if (pThread->GetID() == _thread_id)
			{
				pRes = pThread;

				break;
			}
		}


		_devlib_manage_thread_->UnLock();

		return pRes;
	}

	CThread::listThread CThread::GetThreadList()
	{
		return _devlib_manage_thread_->get();
	}

#if defined(_MSC_VER)
	static unsigned int __stdcall innerThreadCaller(void_ptr _pParent)
#elif defined(__linux__) // Linux
	static void_ptr innerThreadCaller(void_ptr _pParent)
#endif
	{
		auto* pParent = static_cast<CThread*>(_pParent);

		// Register Manage Thread
		_devlib_manage_thread_->Lock();
		_devlib_manage_thread_->get().push_back(pParent);
		_devlib_manage_thread_->UnLock();

		pParent->OnThread();

		// Delete Manage Thread
		_devlib_manage_thread_->Lock();
		_devlib_manage_thread_->get().remove(pParent);
		_devlib_manage_thread_->UnLock();

		//_endthreadex(0);

		return 0;
	}


	CThread::CThread()
		: m_bRunThread(false), m_threadID(0), m_threadHandle(std::make_shared<handle_t>(nullptr))
	{
		std::call_once(_devlib_thread_onceFlag, []()
			{
				_devlib_manage_thread_ = std::unique_ptr<CLocker<listThread>>(new CLocker<listThread>);
			});
	}

	CThread::~CThread()
	{
		if (m_threadHandle.use_count() == 1)
		{
			StopThread();
			WaitForEndThread(3000);
		}
	}

	void CThread::StopThread()
	{
		m_bRunThread = false;
	}

	bool CThread::IsRunThread() const
	{
		return m_bRunThread;
	}

	bool CThread::WaitForEndThread(const uint32_t waitMillisecond)
	{
		bool bRet = true;

#if defined(_MSC_VER)
		const uint32_t _thread_id = _Thrd_id();
#elif defined(__linux__) // Linux
		const uint32_t _thread_id = pthread_self();
#endif
		if (_thread_id == GetID())
		{
			bRet = false;
		}
		else
		{
			if (*m_threadHandle)
			{
#if defined(_MSC_VER)
				if (WaitForSingleObject(*m_threadHandle, waitMillisecond) == WAIT_OBJECT_0)
#elif defined(__linux__) // Linux

				// PTHREAD_CREATE_DETACHED -> pass pthread_join 
				void_ptr threadReturn = nullptr;
				struct timespec curTime;
				clock_gettime(CLOCK_REALTIME, &curTime);

				curTime.tv_sec += static_cast<time_t>(waitMillisecond * 0.001);
				curTime.tv_nsec += static_cast<__syscall_slong_t>(waitMillisecond % 1000 * 1000 * 1000);
				if (pthread_timedjoin_np(reinterpret_cast<pthread_t>(*m_threadHandle), &threadReturn, &curTime) == 0)
					// if (pthread_join(reinterpret_cast<pthread_t>(*m_threadHandle), &threadReturn) == 0)
#endif
				{
					*m_threadHandle = nullptr;
					m_threadID = 0;
				}
				else
				{
					bRet = false;
				}
			}
		}

		return bRet;
	}

	void CThread::OnThread()
	{
		if (m_uptrThreadCaller)
		{
			m_uptrThreadCaller->call();

#if defined(_MSC_VER)
#elif defined(__linux__) // Linux

			// PTHREAD_CREATE_DETACHED -> pass pthread_join 
			*m_threadHandle = nullptr;
			m_threadID = 0;
#endif
		}
		m_bRunThread = false;


	}

	void CThread::SleepMillisecond(size_t waitMillisecond)
	{
#if defined(_MSC_VER)

		Sleep(static_cast<DWORD>(waitMillisecond));

#elif defined(__linux__) // Linux
		usleep(waitMillisecond*1000);
		//std::this_thread::sleep_for(std::chrono::microseconds(waitMicro));

#endif 
	}

	void CThread::SleepMicrosecond(size_t waitMicro)
	{
#if defined(_MSC_VER)
		LARGE_INTEGER timeStart;
		LARGE_INTEGER timeCurrent;
		LARGE_INTEGER freq;

		QueryPerformanceCounter(&timeStart);
		QueryPerformanceFrequency(&freq);

		do
		{
			QueryPerformanceCounter(&timeCurrent);
		} while (static_cast<double_t>(timeCurrent.QuadPart - timeStart.QuadPart) / static_cast<double_t>(freq.QuadPart) * 1000000 < waitMicro);
#elif defined(__linux__) // Linux
		usleep(waitMicro);
		//std::this_thread::sleep_for(std::chrono::microseconds(waitMicro));

#endif 
	}

	bool CThread::BeginThread()
	{
		bool bRet = false;

#if defined(_MSC_VER)
		*m_threadHandle = reinterpret_cast<void*>(_beginthreadex(nullptr, 0, innerThreadCaller, this, 0, &m_threadID));
#elif defined(__linux__) // Linux
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

		if (pthread_create(reinterpret_cast<pthread_t*>(m_threadHandle.get()), &attr, innerThreadCaller, this) == 0)
#endif
			if (*m_threadHandle)
			{
#if defined(_MSC_VER)
#elif defined(__linux__) // Linux
				m_threadID = static_cast<uint32_t>(reinterpret_cast<uint64_t>(*m_threadHandle));
				pthread_attr_destroy(&attr);
#endif
				bRet = true;
				m_bRunThread = true;
			}

		return bRet;
	}
}
