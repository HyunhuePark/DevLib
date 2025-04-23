#pragma once

#include "../Base/CLocker.hpp"
#include "../Base/CThread.hpp"
#include "../Base/CEvent.hpp"
#include "CFile.hpp"
#include <queue>

namespace DevLib {
	namespace IO {

		class CLoggerString final
		{
		public:
			CLoggerString();
			~CLoggerString();

			bool Create(const std::string& logFileName);
			void Destroy();

			bool WriteString(const std::string& str);

		private:
			CFile m_file;
			CThread m_logThread;
			CEvent m_eLogUpdate;

			CLocker<std::queue<std::string>> m_logQueue;

			void LogThread();

		public:
			CLoggerString(const CLoggerString&) = delete;
			CLoggerString(CLoggerString&&) = default;

			CLoggerString& operator=(const CLoggerString&) = delete;
			CLoggerString& operator=(CLoggerString&&) = default;

		};
	}
}
