#pragma once

#include "CCriticalSectionEx.hpp"

namespace DevLib {

	template < class Type >

	class CLocker : public CCriticalSectionEx
	{
	public:
		CLocker() = default;
		CLocker(const CLocker& other) = default;
		CLocker(CLocker&&) = default;

		CLocker& operator=(CLocker& other)
		{
			other.LockRead();
			Lock();
			m_data = *other;
			UnLock();
			other.UnLockRead();

			return *this;
		}

		CLocker& operator=(CLocker&& other) noexcept
		{
			Lock();
			m_data = *other;
			UnLock();

			return *this;
		}

		Type& get() noexcept
		{
			return m_data;
		}

		Type& operator*() noexcept
		{
			return m_data;
		}

		Type* operator->() noexcept
		{
			return &m_data;
		}

		operator Type& () noexcept
		{
			return m_data;
		}

		CLocker& operator=(const Type& other) noexcept
		{
			Lock();
			m_data = other;
			UnLock();

			return *this;
		}

	private:
		Type m_data;

	};

}
