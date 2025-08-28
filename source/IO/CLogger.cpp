#include "../../include/IO/CLogger.hpp"

#if defined( _MSC_VER )
#elif defined(__linux__) // Linux
#include <cstring>
#endif
//////////////////////////////////////////////////////////////////////////
namespace DevLib {
	namespace IO {

		CLogger::CLogger()
		{
			m_eLogUpdate.Create();
		}

		CLogger::~CLogger()
		{
			Destroy();
		}

		bool CLogger::Create(const std::string& logFileName)
		{
			bool bRet = false;

			if (!m_file.IsOpenFile())
			{
				bRet = m_file.Open(logFileName, "wb");

				if (bRet)
				{
					m_logThread.StartThread(&CLogger::LogThread, this);
				}
			}

			return bRet;
		}

		void CLogger::Destroy()
		{
			m_logThread.StopThread();
			m_logThread.WaitForEndThread(1000);
			m_file.Close();

			// Clear Queue
			m_logQueue.Lock();
			std::queue<LogBlob> empty;
			std::swap(*m_logQueue, empty);
			m_logQueue.UnLock();
		}

		bool CLogger::WriteLog(void_const_ptr pData, uint32_t size)
		{
			bool bRet = false;

			if (m_file.IsOpenFile())
			{
				m_logQueue.Lock();
				m_logQueue->emplace();
				m_logQueue->back().resize(size);
				memcpy(m_logQueue->back().data(), pData, size);
				m_logQueue.UnLock();

				bRet = m_eLogUpdate.SetEvent();
			}

			return bRet;
		}

		void CLogger::LogThread()
		{
			while (m_logThread.IsRunThread())
			{
				if (m_eLogUpdate.WaitForEvent(1000))
				{
					m_logQueue.Lock();
					std::queue<LogBlob> blobs = std::move(*m_logQueue);
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
