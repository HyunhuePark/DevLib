#pragma once

#include "DevLibTypes.hpp"

namespace DevLib {

	class CMutex
	{
	public:
		CMutex();
		~CMutex();

		bool Create(const std::string& mutexName = std::string(), bool bDefault = false) const;

		void Destroy() const;

		bool IsCreated() const;

		bool Lock(uint32_t waitMillisecond = 0xFFFFFFFF) const;

		bool UnLock() const;

	private:
		mutexHandle m_hadleMutex;

	public:
		CMutex(const CMutex&) = default;
		CMutex(CMutex&&) = default;

		CMutex& operator=(const CMutex&) = default;
		CMutex& operator=(CMutex&&) = default;

	};
}

