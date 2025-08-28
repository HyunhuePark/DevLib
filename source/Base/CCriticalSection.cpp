#include "../../include/Base/CCriticalSection.hpp"

#if defined(_MSC_VER)
#pragma warning(disable : 5105)
#include <Windows.h>
#pragma warning(default : 5105)
#elif defined(__linux__) // Linux
#endif

namespace DevLib
{
	CCriticalSection::CCriticalSection()
#if defined(_MSC_VER)
		: m_hCriticalSection(new CRITICAL_SECTION)
	{
		InitializeCriticalSection(m_hCriticalSection.get());
#elif defined(__linux__) // Linux
		: m_hCriticalSection(new pthread_spinlock_t)
	{
		pthread_spin_init(m_hCriticalSection.get(), PTHREAD_PROCESS_PRIVATE);
#endif
	}

	CCriticalSection::~CCriticalSection()
	{
		if (m_hCriticalSection.use_count() == 1)
		{
#if defined(_MSC_VER)
			DeleteCriticalSection(m_hCriticalSection.get());
#elif defined(__linux__) // Linux
			pthread_spin_destroy(m_hCriticalSection.get());
#endif
		}
	}

	void CCriticalSection::Lock() const
	{
#if defined(_MSC_VER)
		EnterCriticalSection(m_hCriticalSection.get());
#elif defined(__linux__) // Linux
		pthread_spin_lock(m_hCriticalSection.get());
#endif
	}

	bool CCriticalSection::TryLock() const
	{
#if defined(_MSC_VER)
		return TryEnterCriticalSection(m_hCriticalSection.get()) == TRUE ? true : false;
#elif defined(__linux__) // Linux
		return pthread_spin_trylock(m_hCriticalSection.get()) == 0 ? true : false;
#endif
	}

	void CCriticalSection::UnLock() const
	{
#if defined(_MSC_VER)
		LeaveCriticalSection(m_hCriticalSection.get());
#elif defined(__linux__) // Linux
		pthread_spin_unlock(m_hCriticalSection.get());
#endif
	}

	bool CCriticalSection::IsLocked() const
	{
#if defined(_MSC_VER)
		return reinterpret_cast<CRITICAL_SECTION*>(m_hCriticalSection.get())->RecursionCount ? true : false;
#elif defined(__linux__) // Linux
		bool bRet = true;
		if (TryLock())
		{
			UnLock();
			bRet = false;
		}

		return bRet;
#endif
	}

	}