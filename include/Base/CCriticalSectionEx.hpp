#pragma once

#include "DevLibTypes.hpp"

namespace DevLib
{
	enum class CriticalSectionLockMode
	{
		ReadOnly,
		ReadWrite
	};

	class CCriticalSectionEx
	{
	public:
		CCriticalSectionEx();
		~CCriticalSectionEx();

		void Lock(CriticalSectionLockMode lockMode = CriticalSectionLockMode::ReadWrite) const;
		void LockRead() const { Lock(CriticalSectionLockMode::ReadOnly); }

		bool TryLock(CriticalSectionLockMode lockMode = CriticalSectionLockMode::ReadWrite) const;
		bool TryLockRead() const { return TryLock(CriticalSectionLockMode::ReadOnly); }

		void UnLock(CriticalSectionLockMode lockMode = CriticalSectionLockMode::ReadWrite) const;
		void UnLockRead() const { UnLock(CriticalSectionLockMode::ReadOnly); }

		bool IsLocked(CriticalSectionLockMode lockMode = CriticalSectionLockMode::ReadWrite) const;
		bool IsLockedRead() const { return IsLocked(CriticalSectionLockMode::ReadOnly); }

	private:
		criticalSectionRWHandle m_handleCSEx;


	public:
		CCriticalSectionEx(const CCriticalSectionEx&) = default;
		CCriticalSectionEx(CCriticalSectionEx&&) = default;

		CCriticalSectionEx& operator=(const CCriticalSectionEx&) = default;
		CCriticalSectionEx& operator=(CCriticalSectionEx&&) = default;
	};

}
