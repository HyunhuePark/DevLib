#pragma once

#include "../Base/CLocker.hpp"
#include "../Base/CThread.hpp"
#include "../Base/CEvent.hpp"
#include "CFile.hpp"
#include <queue>

namespace DevLib {
	namespace IO {

		class CLogger final
		{
		public:
			CLogger();
			~CLogger();

			bool Create(const std::string& logFileName);
			void Destroy();

			bool WriteLog(void_const_ptr pData, uint32_t size);

		private:
			CFile m_file;
			CThread m_logThread;
			CEvent m_eLogUpdate;
			using LogBlob = std::vector<uint8_t>;
			CLocker<std::queue<LogBlob>> m_logQueue;

			void LogThread();

		public:
			CLogger(const CLogger&) = delete;
			CLogger(CLogger&&) = default;

			CLogger& operator=(const CLogger&) = delete;
			CLogger& operator=(CLogger&&) = default;

		};
	}
}
