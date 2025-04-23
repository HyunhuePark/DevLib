#pragma once

#include <queue>

#include "../../Base/CLocker.hpp"
#include "../../Base/CVariadicCaller.hpp"
#include "../../IO/CSocketReceiver.hpp"
#include "../../Base/CEvent.hpp"
#include "../Template/CSingletonT.hpp"

namespace DevLib {
	namespace Routine
	{
		class CProcess final : public CSingletonT<CProcess>
		{
		public:
			CProcess() = default;
			~CProcess() = default;

			CProcess(const CProcess&) = delete;
			CProcess& operator=(const CProcess&) = delete;

			bool Init();
			int32_t Run();

			static void Exit();

			static size_t GetThreadTotalCount();
			static std::list<std::string > GetThreadInfo();

			static bool PushNotify(const std::shared_ptr<CAbstractCaller>& notify);

		private:
			CEvent m_eDestroy;

			using QueueNotify = std::queue<std::shared_ptr<CAbstractCaller>>;
			CThread m_threadNotify;
			CEvent m_eNotify;
			CLocker<QueueNotify> m_queueNotify;
			void WatchdogNotify();

		};

	}
}