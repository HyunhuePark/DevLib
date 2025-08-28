#pragma once

#include "../Base/DevLibTypes.hpp"
#include "../Base/CThread.hpp"
#include "../Base/CEvent.hpp"
#include "../Base/DevLibCallback.hpp"

namespace DevLib {
	namespace IO {
		class CSerial
		{
		public:
			CSerial();
			virtual ~CSerial();

			bool OpenPort(const std::string& PortName, uint32_t baudRate, uint8_t byteSize = 8, uint8_t stopBit = 1, uint8_t parity = 0);
			void ClosePort() const;
			bool IsOpened() const;

			bool WaitForRecv(uint32_t ms = 0xFFFFFFFF) const;

			int32_t Recv(void* dst, int size) const;
			int32_t Send(void* src, int size) const;

			std::string GetPortName();
			uint32_t GetBaudRate() const;

			bool SetTimeouts(int readTimeout, int writeTimeout, int readIntervalTimeout) const;

			bool StartWatchDog();
			void StopWatchDog();

			EnableCallback(Receive, CSerial& serial)

		private:
			serialHandle m_hComm;
			std::string m_portNmae = std::string();

			uint32_t	m_baudRate{};
			CEvent		m_eWatch;
			CEvent		m_eRecv;
			CEvent		m_eSend;

			CThread		m_serialThread;
			void WatchDogRecv(); // watchdog for recv

		public:
			CSerial(const CSerial& othrer) = delete;
			CSerial(CSerial&&) = default;

			CSerial& operator=(const CSerial&) = delete;
			CSerial& operator=(CSerial&&) = default;
		};
	}
};