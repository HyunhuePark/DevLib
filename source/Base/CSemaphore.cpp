#include "../../include/Base/CSemaphore.hpp"

#if defined(_MSC_VER)
#pragma warning(disable : 5105)
#include <Windows.h>
#pragma warning(default : 5105)
#elif defined(__linux__) // Linux
#include <semaphore.h>
#include <fcntl.h>
#endif

namespace DevLib {

	CSemaphore::CSemaphore()
		: m_handleSemaphore(std::make_shared<void*>(nullptr))
		, m_nameSemaphore(std::string())
	{
	}

	CSemaphore::~CSemaphore()
	{
		if (m_handleSemaphore.use_count() == 1)
		{
			Destroy();
		}
	}

	bool CSemaphore::Create(const int32_t KeyCount /*= 1*/, const std::string& semaName /*= std::string()*/)
	{
		bool bRet = false;

		if (!IsCreated())
		{

#if defined(_MSC_VER)
			std::string name;
			if (!semaName.empty())
			{
				name = std::string("Global\\_DevLib_Semaphore_") + semaName;
				m_nameSemaphore = semaName;
			}

			*m_handleSemaphore = OpenSemaphoreA(SEMAPHORE_ALL_ACCESS, FALSE, name.c_str());

			if (!IsCreated()) *m_handleSemaphore = CreateSemaphoreA(nullptr, KeyCount, KeyCount, name.c_str());
#elif defined(__linux__) // Linux
			if ((*m_handleSemaphore = sem_open(semaName.c_str(), O_CREAT | O_EXCL, 0644, KeyCount)) == SEM_FAILED)
			{
				if (errno == EEXIST) // open
				{
					*m_handleSemaphore = sem_open(semaName.c_str(), 0, 0644, 0);
					m_nameSemaphore = std::string(semaName);
				}
			}
#endif
			if (IsCreated()) bRet = true;
		}

		return bRet;
	}

	void CSemaphore::Destroy() const
	{
		if (IsCreated())
		{
#if defined(_MSC_VER)
			ReleaseSemaphore(*m_handleSemaphore, 1, nullptr);
			CloseHandle(*m_handleSemaphore);
#elif defined(__linux__) // Linux
			(void)sem_close(reinterpret_cast<sem_t*>(m_handleSemaphore.get()));
			//(void)sem_unlink(m_nameSemaphore.c_str()); // remove /dev/shm
#endif
			* m_handleSemaphore = nullptr;

		}
	}

	bool CSemaphore::IsCreated() const
	{
		return *m_handleSemaphore ? true : false;
	}

	bool CSemaphore::WaitForSemaphore(const uint32_t waitMillisecond /*= 0xFFFFFFFF*/) const
	{
#if defined(_MSC_VER)
		return WaitForSingleObject(*m_handleSemaphore, waitMillisecond) == WAIT_OBJECT_0 ? true : false;
#elif defined(__linux__) // Linux
		bool bRet = false;

		timespec _wait;

		(void)clock_gettime(CLOCK_REALTIME, &_wait);

		const float_t dsec = (static_cast<float_t>(waitMillisecond) * 0.001F);
		const int64_t nsec = waitMillisecond % 1000 * 1000000;
		const int64_t sec = static_cast<int64_t>(dsec);
		_wait.tv_sec += sec;
		_wait.tv_nsec += nsec;
		if (_wait.tv_nsec > 1000000000)
		{
			_wait.tv_sec += 1;
			_wait.tv_nsec -= 1000000000;
		}

		if (sem_timedwait(reinterpret_cast<sem_t*>(m_handleSemaphore.get()), &_wait) == 0)
		{
			bRet = true;
		}

		return bRet;
#endif
	}

	bool CSemaphore::ReturnSemaphore(const int32_t nKey /*= 1*/) const
	{
#if defined(_MSC_VER)
		return ReleaseSemaphore(*m_handleSemaphore, nKey, nullptr) ? true : false;
#elif defined(__linux__) // Linux
		return sem_post(reinterpret_cast<sem_t*>(m_handleSemaphore.get())) == 0 ? true : false;
#endif
	}

	int32_t CSemaphore::GetCurrentKey() const
	{
#if defined(_MSC_VER)
		LONG count = 0;
		ReleaseSemaphore(*m_handleSemaphore, 0, &count);
#elif defined(__linux__) // Linux
		int32_t count = 0;
		sem_getvalue(reinterpret_cast<sem_t*>(m_handleSemaphore.get()), &count);
#endif
		return count;
	}
}
