#include "../../include/IO/CLoggerString.hpp"

#include "../../include/Base/CTime.hpp"
//////////////////////////////////////////////////////////////////////////
namespace DevLib {
	namespace IO {

		CLoggerString::CLoggerString()
		{
			m_eLogUpdate.Create();
		}

		CLoggerString::~CLoggerString()
		{
			Destroy();
		}

		bool CLoggerString::Create(const std::string& logFileName)
		{
			bool bRet = false;

			if (!m_file.IsOpenFile())
			{
				bRet = m_file.Open(logFileName, "wt");

				if (bRet)
				{
					m_logThread.StartThread(&CLoggerString::LogThread, this);
				}
			}

			return bRet;
		}

		void CLoggerString::Destroy()
		{
			m_logThread.StopThread();
			m_logThread.WaitForEndThread(1000);
			m_file.Close();

			// Clear Queue
			m_logQueue.Lock();
			std::queue<std::string> empty;
			std::swap(*m_logQueue, empty);
			m_logQueue.UnLock();
		}

		bool CLoggerString::WriteString(const std::string& str)
		{
			bool bRet = false;

			if (m_file.IsOpenFile())
			{
				const CTime current;
				m_logQueue.Lock();
				m_logQueue->emplace();
				m_logQueue->back() = current.GetTimeToString() + std::string(" : ") + str + std::string("\n");
				m_logQueue.UnLock();

				bRet = m_eLogUpdate.SetEvent();
			}

			return bRet;
		}

		void CLoggerString::LogThread()
		{
			while (m_logThread.IsRunThread())
			{
				if (m_eLogUpdate.WaitForEvent(1000))
				{
					m_logQueue.Lock();
					std::queue<std::string> blobs = std::move(*m_logQueue);
					m_logQueue.UnLock();

					while (!blobs.empty())
					{
						m_file.Write(blobs.front().data(), blobs.front().size());
						m_file.Flush();

						blobs.pop();

					}
				}
			}
		}
	}
}
