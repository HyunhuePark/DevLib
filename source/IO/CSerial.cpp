#include "../../include/IO/CSerial.hpp"

#if defined(_MSC_VER)
#pragma warning(disable : 5105)
#include <Windows.h>
#pragma warning(default : 5105)
#elif defined(__linux__) // Linux
#include <fcntl.h>
#include <termios.h>
#include <cstring>
#include <unistd.h>
#include <sys/epoll.h>
#endif

namespace DevLib
{
	namespace IO
	{

		constexpr uint32_t SizeSerialBuffer_ = 8192;

#if defined(_MSC_VER)
#elif defined(__linux__) // Linux
		constexpr int32_t INVALID_HANDLE_VALUE = -1;
#endif

		//////////////////////////////////////////////////////////////////////////
		// CCommSerial
		CSerial::CSerial()
#if defined(_MSC_VER)
			: m_hComm(std::make_shared<handle_t>(INVALID_HANDLE_VALUE))
#elif defined(__linux__) // Linux
			: m_hComm(std::make_shared<int32_t>(INVALID_HANDLE_VALUE))
			, m_baudRate(115200)
#endif
		{
			RegisterCallbackReceive(&CSerial::OnReceive, this);

			m_eWatch.Create(true, false);
			m_eRecv.Create(true, false);
			m_eSend.Create(true, false);
		}

		CSerial::~CSerial()
		{
			if (m_hComm.use_count() == 1)
			{
				ClosePort();
			}
		}

		bool CSerial::OpenPort(const std::string &PortName, uint32_t baudRate, uint8_t byteSize /*= 8*/, uint8_t stopBit /*= 1*/, uint8_t parity /*= 0*/)
		{
			bool bRet = false;

			// Serial
#if defined(_MSC_VER)
			*m_hComm = CreateFileA(PortName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr,
								   OPEN_EXISTING, FILE_FLAG_OVERLAPPED, nullptr);
#elif defined(__linux__) // Linux
			*m_hComm = open(PortName.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK );
#endif

			if (*m_hComm != INVALID_HANDLE_VALUE)
			{
#if defined(_MSC_VER)

#define ASCII_XON 0x11
#define ASCII_XOFF 0x13
				SetCommMask(*m_hComm, EV_RXCHAR);

				SetupComm(*m_hComm, SizeSerialBuffer_, SizeSerialBuffer_);

				PurgeComm(*m_hComm, PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR);

				COMMTIMEOUTS timeouts;
				timeouts.ReadIntervalTimeout = 0xFFFFFFFF;
				timeouts.ReadTotalTimeoutMultiplier = 0;
				timeouts.ReadTotalTimeoutConstant = 0;
				if (baudRate > 0)
					timeouts.WriteTotalTimeoutMultiplier = (2 * CBR_9600) / baudRate;
				else
					timeouts.WriteTotalTimeoutMultiplier = CBR_9600;
				timeouts.WriteTotalTimeoutConstant = 0;
				SetCommTimeouts(*m_hComm, &timeouts);

				DCB dcb;
				dcb.DCBlength = sizeof(DCB);

				GetCommState(*m_hComm, &dcb);
				dcb.BaudRate = static_cast<unsigned long>(baudRate);
				dcb.ByteSize = byteSize;
				dcb.Parity = parity;
				dcb.StopBits = stopBit == 1 ? 0 : 1; // dcb.StopBits : 0 = 1 bit, 1 = 1.5bit, 2 = 2bit
				dcb.fInX = dcb.fOutX = 0;
				dcb.XonChar = ASCII_XON;
				dcb.XoffChar = ASCII_XOFF;
				dcb.XonLim = 100;
				dcb.XoffLim = 100;

				if (SetCommState(*m_hComm, &dcb))
				{
					m_portNmae = PortName;
					m_baudRate = baudRate;

					bRet = true;
				}
				else
					printf("SetCommState Error\n");

#elif defined(__linux__) // Linux
				m_portNmae = PortName;
				termios termiosConfig;
				bzero(&termiosConfig, sizeof(termiosConfig));

				tcflag_t _baudRate = 0;
				switch (baudRate)
				{
				case 0:
					_baudRate = B0;
					break;
				case 50:
					_baudRate = B50;
					break;
				case 75:
					_baudRate = B75;
					break;
				case 110:
					_baudRate = B110;
					break;
				case 134:
					_baudRate = B134;
					break;
				case 150:
					_baudRate = B150;
					break;
				case 200:
					_baudRate = B200;
					break;
				case 300:
					_baudRate = B300;
					break;
				case 600:
					_baudRate = B600;
					break;
				case 1200:
					_baudRate = B1200;
					break;
				case 1800:
					_baudRate = B1800;
					break;
				case 2400:
					_baudRate = B2400;
					break;
				case 4800:
					_baudRate = B4800;
					break;
				case 9600:
					_baudRate = B9600;
					break;
				case 19200:
					_baudRate = B19200;
					break;
				case 38400:
					_baudRate = B38400;
					break;
				case 57600:
					_baudRate = B57600;
					break;
				case 115200:
					_baudRate = B115200;
					break;
				case 230400:
					_baudRate = B230400;
					break;
				case 460800:
					_baudRate = B460800;
					break;
				case 500000:
					_baudRate = B500000;
					break;
				case 576000:
					_baudRate = B576000;
					break;
				case 921600:
					_baudRate = B921600;
					break;
				case 1000000:
					_baudRate = B1000000;
					break;
				case 1152000:
					_baudRate = B1152000;
					break;
				case 1500000:
					_baudRate = B1500000;
					break;
				case 2000000:
					_baudRate = B2000000;
					break;
				case 2500000:
					_baudRate = B2500000;
					break;
				case 3000000:
					_baudRate = B3000000;
					break;
				case 3500000:
					_baudRate = B3500000;
					break;
				case 4000000:
					_baudRate = B4000000;
					break;

				default:
					break;
				}

				tcflag_t _byteSize = CS8;
				switch (byteSize)
				{
				case 5 :
					_byteSize = CS5;
					break;
				case 6 :
					_byteSize = CS6;
					break;
				case 7 :
					_byteSize = CS7;
					break;
				case 8 :
					_byteSize = CS8;
					break;
				
				default:
					break;
				}

				// termiosConfig.c_cflag = _baudRate | _byteSize | CLOCAL | CRTSCTS | CREAD;	// Default Option
				termiosConfig.c_cflag = _baudRate | _byteSize | CLOCAL | CREAD;	// Default Option
				termiosConfig.c_oflag = 0;
				termiosConfig.c_iflag = IGNPAR; // ICRNL
				// termiosConfig.c_lflag = ICANON;
				termiosConfig.c_cc[VINTR] = 0;	  /* Ctrl-c */
				termiosConfig.c_cc[VQUIT] = 0;	  /* Ctrl-\ */
				termiosConfig.c_cc[VERASE] = 0;	  /* del */
				termiosConfig.c_cc[VKILL] = 0;	  /* @ */
				termiosConfig.c_cc[VEOF] = 4;	  /* Ctrl-d */
				termiosConfig.c_cc[VTIME] = 0;	  /* inter-character timer unused */
				termiosConfig.c_cc[VMIN] = 1;	  /* blocking read until 1 character arrives */
				termiosConfig.c_cc[VSWTC] = 0;	  /* '\0' */
				termiosConfig.c_cc[VSTART] = 0;	  /* Ctrl-q */
				termiosConfig.c_cc[VSTOP] = 0;	  /* Ctrl-s */
				termiosConfig.c_cc[VSUSP] = 0;	  /* Ctrl-z */
				termiosConfig.c_cc[VEOL] = 0;	  /* '\0' */
				termiosConfig.c_cc[VREPRINT] = 0; /* Ctrl-r */
				termiosConfig.c_cc[VDISCARD] = 0; /* Ctrl-u */
				termiosConfig.c_cc[VWERASE] = 0;  /* Ctrl-w */
				termiosConfig.c_cc[VLNEXT] = 0;	  /* Ctrl-v */
				termiosConfig.c_cc[VEOL2] = 0;	  /* '\0' */

				(void)tcflush(*m_hComm, TCIFLUSH);
				(void)tcsetattr(*m_hComm, TCSANOW, &termiosConfig);

				bRet = true;
#endif
			}

			//////////////////////////////////////////////////////////////////////////

			return bRet;
		}

		void CSerial::ClosePort() const
		{
			if (*m_hComm != INVALID_HANDLE_VALUE)
			{
#if defined(_MSC_VER)
				SetCommMask(*m_hComm, 0);
				PurgeComm(*m_hComm, PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR);
				CloseHandle(*m_hComm);
#elif defined(__linux__) // Linux
				(void)close(*m_hComm);
#endif

				*m_hComm = INVALID_HANDLE_VALUE;
			}
		}

		bool CSerial::IsOpened() const
		{
			return *m_hComm != INVALID_HANDLE_VALUE ? true : false;
		}

		bool CSerial::WaitForRecv(uint32_t ms /*= 0xFFFFFFFF*/) const
		{
			bool bRet = false;
			bool bWork = true;

			// Watchdog
			if (m_serialThread.IsRunThread())
			{
				const CThread *pCurrentThread = CThread::GetCurrentThread();

				if (pCurrentThread)
				{
					if (pCurrentThread->GetID() != m_serialThread.GetID())
					{
						bWork = false;
					}
				}
				else
				{
					bWork = false;
				}
			}

			if (IsOpened() && bWork)
			{
#if defined(_MSC_VER)
				OVERLAPPED ovWatch = {};
				ovWatch.hEvent = m_eWatch.GetEventHandle();

				DWORD dwEvent = 0;
				WaitCommEvent(*m_hComm, &dwEvent, &ovWatch);
				bRet = m_eWatch.WaitForEvent(ms);
#elif defined(__linux__) // Linux
				const int32_t fdRecv = epoll_create(1);

				struct epoll_event ev;
				struct epoll_event evValue;

				ev.events = EPOLLIN | EPOLLET;
				ev.data.fd = *m_hComm;

				if (epoll_ctl(fdRecv, EPOLL_CTL_ADD, *m_hComm, &ev) != -1)
				{
					if (epoll_wait(fdRecv, &evValue, 1, ms) != -1) // wait Time ms
					{
						const uint32_t epollin = EPOLLIN;
						if ((evValue.events & epollin /*|| evValue.events & EPOLLET*/) > 0)
						{
							bRet = true;
						}
					}

					// remove
					epoll_ctl(fdRecv, EPOLL_CTL_DEL, *m_hComm, nullptr);

					close(fdRecv);
				}
#endif
			}

			return bRet;
		}

		std::string CSerial::GetPortName()
		{
			return {m_portNmae};
		}

		uint32_t CSerial::GetBaudRate() const
		{
			return m_baudRate;
		}

		int32_t CSerial::Recv(void *dst, int size) const
		{
#if defined(_MSC_VER)
			OVERLAPPED osRead = {};
			osRead.hEvent = m_eRecv.GetEventHandle();

			DWORD dwRead = 0;

			if (!ReadFile(*m_hComm, dst, size, &dwRead, &osRead))
			{
				if (GetLastError() == ERROR_IO_PENDING)
				{
					if (!GetOverlappedResult(*m_hComm, &osRead, &dwRead, TRUE))
					{
						if (GetLastError() != ERROR_IO_INCOMPLETE)
						{
							DWORD dwErrorFlags;
							COMSTAT comstat;
							ClearCommError(*m_hComm, &dwErrorFlags, &comstat);
						}
					}
				}
			}
#elif defined(__linux__) // Linux
			int32_t dwRead = 0;

			dwRead = read(*m_hComm, dst, size);
#endif

			return static_cast<int32_t>(dwRead);
		}

		int32_t CSerial::Send(void *src, int size) const
		{
#if defined(_MSC_VER)
			OVERLAPPED osSend = { };
			osSend.hEvent = m_eSend.GetEventHandle();

			DWORD dwWritten = 0;

			if (!WriteFile(*m_hComm, src, size, &dwWritten, &osSend))
			{
				if (GetLastError() == ERROR_IO_PENDING)
				{
					if (!GetOverlappedResult(*m_hComm, &osSend, &dwWritten, TRUE))
					{
						if (GetLastError() != ERROR_IO_INCOMPLETE)
						{
							DWORD dwErrorFlags;
							COMSTAT comstat;
							ClearCommError(*m_hComm, &dwErrorFlags, &comstat);
						}
					}
				}
			}
#elif defined(__linux__) // Linux
			int32_t dwWritten = 0;
			dwWritten = write(*m_hComm, src, size);
#endif
			return static_cast<int32_t>(dwWritten);
		}

		bool CSerial::SetTimeouts(int readTimeout, int writeTimeout, int readIntervalTimeout) const
		{
			bool bRet = false;
#if defined(_MSC_VER)

			COMMTIMEOUTS commTimeout;

			if (!GetCommTimeouts(*m_hComm, &commTimeout))
			{
				return false;
			}

			commTimeout.ReadIntervalTimeout = readIntervalTimeout;
			commTimeout.ReadTotalTimeoutMultiplier = 1;
			commTimeout.ReadTotalTimeoutConstant = readTimeout;
			commTimeout.WriteTotalTimeoutMultiplier = 1;
			commTimeout.WriteTotalTimeoutConstant = writeTimeout;
			bRet = SetCommTimeouts(*m_hComm, &commTimeout) == TRUE ? true : false;
#elif defined(__linux__) // Linux

#endif
			return bRet;
		}

		bool CSerial::StartWatchDog()
		{
			return m_serialThread.StartThread(&CSerial::WatchDogRecv, this);
		}

		void CSerial::StopWatchDog()
		{
			m_serialThread.StopThread();
		}

		void CSerial::WatchDogRecv()
		{
			while (m_serialThread.IsRunThread())
			{
				if (WaitForRecv(1000))
				{
					m_callbackReceive(*this);
				}
			}
		}

	}
};
