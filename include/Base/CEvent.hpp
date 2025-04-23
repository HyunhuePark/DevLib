#pragma once

#include "DevLibTypes.hpp"

namespace DevLib {

	class CEvent
	{
	public:
		CEvent();
		~CEvent();

		bool Create(bool bManual = false, bool bInitSignal = false, const std::string& eventName = std::string());

		void Destroy() const;

		bool SetEvent() const;

		bool ResetEvent() const;

		bool WaitForEvent(uint32_t waitMillisecond = 0xFFFFFFFF) const;

		bool IsCreated() const;

		bool IsManual() const;

		bool IsSignaled() const;

		event_t GetEventHandle() const { return *m_handleEvent; }

	protected:
		std::shared_ptr<event_t> m_handleEvent;
		bool m_bManual;

	public:
		CEvent(const CEvent&) = default;
		CEvent(CEvent&&) = default;

		CEvent& operator=(const CEvent&) = default;
		CEvent& operator=(CEvent&&) = default;
	};

}
