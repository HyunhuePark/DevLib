#pragma once

#include "DevLibTypes.hpp"

namespace DevLib {

	class CCriticalSection final
	{
	public:
		CCriticalSection();
		~CCriticalSection();

		void Lock() const;

		bool TryLock() const;

		void UnLock() const;

		bool IsLocked() const;

	private:
		criticalSectionHandle m_hCriticalSection;

	public:
		CCriticalSection(const CCriticalSection&) = default;
		CCriticalSection(CCriticalSection&&) = default;

		CCriticalSection& operator=(const CCriticalSection&) = default;
		CCriticalSection& operator=(CCriticalSection&&) = default;
	};

}


