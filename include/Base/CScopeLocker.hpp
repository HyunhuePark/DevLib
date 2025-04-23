#pragma once

#include "CCriticalSection.hpp"
#include "CCriticalSectionEx.hpp"
#include "CMutex.hpp"

namespace DevLib {

	template < typename Type, typename = std::enable_if_t<
		std::is_same_v<CCriticalSection, Type> ||
		std::is_same_v<CCriticalSectionEx, Type> ||
		std::is_same_v<CMutex, Type>, Type > >
	class CScopeLocker
	{
	public:
		CScopeLocker(Type& locker)
			: m_locker(locker)
		{
			locker.Lock();
		}

		~CScopeLocker()
		{
			m_locker.UnLock();
		}

		Type& get() noexcept
		{
			return m_locker;
		}

		Type& operator*() noexcept
		{
			return m_locker;
		}

		Type* operator->() noexcept
		{
			return &m_locker;
		}

	private:
		Type& m_locker;
	};
}