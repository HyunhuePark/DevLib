#pragma once

#include "DevLibTypes.hpp"

namespace DevLib {

	class CSemaphore
	{
	public:
		CSemaphore();
		~CSemaphore();

		bool Create(int32_t KeyCount = 1, const std::string& semaName = std::string());

		void Destroy() const;

		bool IsCreated() const;

		bool WaitForSemaphore(uint32_t waitMillisecond = 0xFFFFFFFF) const;

		bool ReturnSemaphore(int32_t nKey = 1) const;

		int32_t GetCurrentKey() const;

	private:
		sharedHandle m_handleSemaphore;
		std::string m_nameSemaphore;

	public:
		CSemaphore(const CSemaphore&) = default;
		CSemaphore(CSemaphore&&) = default;

		CSemaphore& operator=(const CSemaphore&) = default;
		CSemaphore& operator=(CSemaphore&&) = default;
	};
}

