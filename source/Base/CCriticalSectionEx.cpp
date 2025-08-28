#include "../../include/Base/CCriticalSectionEx.hpp"

#if defined(_MSC_VER)
#pragma warning(disable : 5105)
#include <Windows.h>
#pragma warning(default : 5105)
#elif defined(__linux__) // Linux

#endif

namespace DevLib
{
	CCriticalSectionEx::CCriticalSectionEx()
#if defined(_MSC_VER)
		: m_handleCSEx(new SRWLOCK)
	{
		InitializeSRWLock(m_handleCSEx.get());
#elif defined(__linux__) // Linux
		: m_handleCSEx(new pthread_rwlock_t)
	{
		pthread_rwlock_init(m_handleCSEx.get(), nullptr);
#endif
	}

	CCriticalSectionEx::~CCriticalSectionEx()
	{
		if (m_handleCSEx.use_count() == 1)
		{
#if defined(_MSC_VER)

#elif defined(__linux__) // Linux
			pthread_rwlock_destroy(m_handleCSEx.get());
#endif
		}
	}

	void CCriticalSectionEx::Lock(const CriticalSectionLockMode lockMode /*= CriticalSectionLockMode::ReadWrite*/) const
	{
		if (lockMode == CriticalSectionLockMode::ReadWrite)
		{
#if defined(_MSC_VER)
			AcquireSRWLockExclusive(m_handleCSEx.get());
#elif defined(__linux__) // Linux
			pthread_rwlock_wrlock(m_handleCSEx.get());
#endif
		}
		else
		{
#if defined(_MSC_VER)
			AcquireSRWLockShared(m_handleCSEx.get());
#elif defined(__linux__) // Linux
			pthread_rwlock_rdlock(m_handleCSEx.get());
#endif
		}
	}

	bool CCriticalSectionEx::TryLock(const CriticalSectionLockMode lockMode /*= CriticalSectionLockMode::ReadWrite*/) const
	{
		bool bRet;
		if (lockMode == CriticalSectionLockMode::ReadWrite)
		{
#if defined(_MSC_VER)
			bRet = TryAcquireSRWLockExclusive(m_handleCSEx.get()) == TRUE ? true : false;
#elif defined(__linux__) // Linux
			bRet = pthread_rwlock_trywrlock(m_handleCSEx.get()) == 0 ? true : false;
#endif
		}
		else
		{
#if defined(_MSC_VER)
			bRet = TryAcquireSRWLockShared(m_handleCSEx.get()) == TRUE ? true : false;
#elif defined(__linux__) // Linux
			bRet = pthread_rwlock_tryrdlock(m_handleCSEx.get()) == 0 ? true : false;
#endif
		}

		return bRet;
	}

	void CCriticalSectionEx::UnLock(const CriticalSectionLockMode lockMode /*= CriticalSectionLockMode::ReadWrite*/) const
	{
		if (lockMode == CriticalSectionLockMode::ReadWrite)
		{
#if defined(_MSC_VER)
			ReleaseSRWLockExclusive(m_handleCSEx.get());
#elif defined(__linux__) // Linux
			pthread_rwlock_unlock(m_handleCSEx.get());
#endif
		}
		else
		{
#if defined(_MSC_VER)
			ReleaseSRWLockShared(m_handleCSEx.get());
#elif defined(__linux__) // Linux
			pthread_rwlock_unlock(m_handleCSEx.get());
#endif
		}
	}

	bool CCriticalSectionEx::IsLocked(const CriticalSectionLockMode lockMode /*= CriticalSectionLockMode::ReadWrite*/) const
	{
		bool bRet = true;
		if (TryLock(lockMode))
		{
			UnLock(lockMode);
			bRet = false;
		}

		return bRet;
	}

	}