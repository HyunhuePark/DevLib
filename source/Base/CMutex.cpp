#include "../../include/Base/CMutex.hpp"

#if defined(_MSC_VER)
#pragma warning(disable : 5105)
#include <Windows.h>
#pragma warning(default : 5105)
#elif defined(__linux__) // Linux
#include <pthread.h>
#endif

namespace DevLib
{

	CMutex::CMutex()
#if defined(_MSC_VER)
		: m_hadleMutex(std::make_shared<handle_t>(nullptr))
	{
	}
#elif defined(__linux__) // Linux
		: m_hadleMutex(std::make_shared<pthread_mutex_t>())
	{
		m_hadleMutex.get()->__align = -1;
	}
#endif

	CMutex::~CMutex()
	{
		if (m_hadleMutex.use_count() == 1)
		{
			Destroy();
		}
	}

#if defined(_MSC_VER)
	bool CMutex::Create(const std::string& mutexName /*= std::string()*/, const bool bDefault /*= false*/) const
#else // Linux
	bool CMutex::Create(const std::string& /*= std::string()*/, const bool /*= false*/) const
#endif
	{
		bool bRet = false;

		if (!IsCreated())
		{
#if defined(_MSC_VER)
			std::string name;
			if (!mutexName.empty())
			{
				name = std::string("Global\\_DevLib_Mutex_") + mutexName;
			}

			*m_hadleMutex = OpenMutexA(MUTEX_ALL_ACCESS, FALSE, name.c_str());
			if (!IsCreated())
				*m_hadleMutex = CreateMutexA(nullptr, bDefault ? TRUE : FALSE, name.c_str());

#elif defined(__linux__) // Linux
			if (pthread_mutex_init(m_hadleMutex.get(), nullptr) != 0)
			{
				Destroy();
			}
#endif

			if (IsCreated())
				bRet = true;
		}

		return bRet;
	}

	void CMutex::Destroy() const
	{
		if (IsCreated())
		{
#if defined(_MSC_VER)
			CloseHandle(*m_hadleMutex);
			*m_hadleMutex = nullptr;
#elif defined(__linux__) // Linux
			pthread_mutex_destroy(m_hadleMutex.get());
#endif	

		}
	}

	bool CMutex::IsCreated() const
	{
#if defined(_MSC_VER)
		return *m_hadleMutex ? true : false;
#elif defined(__linux__) // Linux
		return m_hadleMutex.get()->__align == -1 ? false : true;
#endif
	}

	bool CMutex::Lock(const uint32_t waitMillisecond /*= 0xFFFFFFFF*/) const
	{
#if defined(_MSC_VER)
		return WaitForSingleObject(*m_hadleMutex, waitMillisecond) ? false : true;
#elif defined(__linux__) // Linux
		struct timespec waitTime;
		clock_gettime(CLOCK_REALTIME, &waitTime);
		waitTime.tv_nsec += (waitMillisecond % 1000) * 1000000;
		waitTime.tv_sec += waitMillisecond * 0.001 + waitTime.tv_nsec * 0.000000001;

		if (waitTime.tv_nsec > 1000000000)
		{
			waitTime.tv_sec += 1;
			waitTime.tv_nsec -= 1000000000;
		}

		return pthread_mutex_timedlock(m_hadleMutex.get(), &waitTime) ? false : true;
#endif
	}

	bool CMutex::UnLock() const
	{
#if defined(_MSC_VER)
		return ReleaseMutex(*m_hadleMutex) ? false : true;
#elif defined(__linux__) // Linux
		return pthread_mutex_unlock(m_hadleMutex.get()) ? false : true;
#endif
	}

}