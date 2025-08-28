#include "../../include/Base/CEvent.hpp"

#if defined(_MSC_VER)
#pragma warning(disable : 5105)
#include <Windows.h>
#pragma warning(default : 5105)
#elif defined(__linux__) // Linux
#include <fcntl.h>
#include <ctime>
#endif

namespace DevLib
{

	CEvent::CEvent()
		: m_handleEvent(std::make_shared<event_t>(event_t())), m_bManual(false)
	{
	}

	CEvent::~CEvent()
	{
		if (m_handleEvent.use_count() == 1)
		{
			Destroy();
		}
	}

	bool CEvent::Create(const bool bManual /*= false*/, const bool bInitSignal /*= false*/, const std::string& eventName /*= std::string()*/)
	{
		bool bRet = false;

		if (!IsCreated())
		{
			m_bManual = bManual;

#if defined(_MSC_VER)
			std::string name;
			if (!eventName.empty())
			{
				name = std::string("Global\\_DevLib_Event_") + eventName;
			}

			*m_handleEvent = OpenEventA(EVENT_ALL_ACCESS, FALSE, name.c_str());
			if (!IsCreated())
				*m_handleEvent = CreateEventA(nullptr, bManual, bInitSignal, name.c_str());
#elif defined(__linux__) // Linux
			if (eventName.empty()) //
			{
				if (!pthread_mutex_init(&m_handleEvent->mutex, nullptr))
				{
					if (!pthread_cond_init(&m_handleEvent->cond, nullptr))
					{
						m_handleEvent->bInit = true;
					}
				}
			}
			else // global
			{
				if ((m_handleEvent->sema = sem_open(eventName.c_str(), O_CREAT | O_EXCL, 666, 1)) == SEM_FAILED)
				{
					if (errno == EEXIST) // open
					{
						m_handleEvent->sema = sem_open(eventName.c_str(), 0, 666, 0);

						m_handleEvent->eventName = eventName;

						m_handleEvent->bInit = true;
					}
				}
			}
#endif

			if (IsCreated())
				bRet = true;
		}

		return bRet;
	}

	void CEvent::Destroy() const
	{
#if defined(_MSC_VER)
		if (*m_handleEvent)
		{
			CloseHandle(*m_handleEvent);
			*m_handleEvent = nullptr;
		}
#elif defined(__linux__) // Linux
		if (m_handleEvent->bInit)
		{

			if (m_handleEvent->eventName.empty())
			{
				(void)pthread_mutex_destroy(&m_handleEvent->mutex);
				(void)pthread_cond_destroy(&m_handleEvent->cond);
			}
			else
			{
				(void)sem_close(m_handleEvent->sema);
				(void)sem_unlink(m_handleEvent->eventName.c_str());
			}
		}
#endif
	}

	bool CEvent::SetEvent() const
	{
#if defined(_MSC_VER)
		return ::SetEvent(*m_handleEvent) == TRUE ? true : false;
#elif defined(__linux__) // Linux
		if (m_handleEvent->eventName.empty())
		{
			(void)pthread_mutex_lock(&m_handleEvent->mutex);
			m_handleEvent->bSignal = true;
			(void)pthread_cond_signal(&m_handleEvent->cond);
			(void)pthread_mutex_unlock(&m_handleEvent->mutex);
		}
		else
		{
			int nKey = 0;
			(void)sem_getvalue(m_handleEvent->sema, &nKey);
			if (nKey < 1)
			{
				sem_post(m_handleEvent->sema);
			}
		}

		return true;
#endif
	}

	bool CEvent::ResetEvent() const
	{
#if defined(_MSC_VER)
		return ::ResetEvent(*m_handleEvent) == TRUE ? true : false;
#elif defined(__linux__) // Linux
		if (m_handleEvent->eventName.empty())
		{
			m_handleEvent->bSignal = false;
		}
		else
		{
			sem_trywait(m_handleEvent->sema);
		}

		return true;
#endif
	}

	bool CEvent::WaitForEvent(const uint32_t waitMillisecond /*= 0xFFFFFFFF*/) const
	{
#if defined(_MSC_VER)
		return WaitForSingleObject(*m_handleEvent, waitMillisecond) ? false : true;
#elif defined(__linux__) // Linux
		bool bRet = false;

		if (IsCreated())
		{
			timespec _wait;

			(void)clock_gettime(CLOCK_REALTIME, &_wait);

			const float_t dsec = (static_cast<float_t>(waitMillisecond) * 0.001F);
			const int64_t nsec = waitMillisecond % 1000 * 1000000;
			const int64_t sec = static_cast<int64_t>(dsec);
			_wait.tv_sec += sec;
			_wait.tv_nsec += nsec;
			if( _wait.tv_nsec > 1000000000 )
			{
				_wait.tv_sec += 1;
				_wait.tv_nsec -= 1000000000;
			}
			
			if (m_handleEvent->eventName.empty())
			{
				if (m_handleEvent->bSignal == true)
				{
					bRet = true;
				}
				else
				{
					(void)pthread_mutex_lock(&m_handleEvent->mutex);

					if (pthread_cond_timedwait(&m_handleEvent->cond, &m_handleEvent->mutex, &_wait) == 0)
					{
						bRet = true;
					}

					(void)pthread_mutex_unlock(&m_handleEvent->mutex);
				}

				m_handleEvent->bSignal = false;
			}
			else
			{
				if (sem_timedwait(m_handleEvent->sema, &_wait) == 0)
				{
					bRet = true;
				}
			}
		}
		return bRet;
#endif
	}

	bool CEvent::IsCreated() const
	{
#if defined(_MSC_VER)
		return *m_handleEvent ? true : false;
#elif defined(__linux__) // Linux
		return m_handleEvent->bInit;
#endif
	}

	bool CEvent::IsManual() const
	{
		return m_bManual;
	}

	bool CEvent::IsSignaled() const
	{
		const bool ret = WaitForEvent(0);

		if (ret && m_bManual)
		{
			(void)ResetEvent();
			(void)SetEvent();
		}
		else if (ret && m_bManual == false)
		{
			(void)SetEvent();
		}
		else
			;

		return ret;
	}
}
