#include "../../../include/Routine/Process/CProcess.hpp"
#include "../../../include/Base/CThread.hpp"

#include "../../../include/IO/DevLibSocketUtility.hpp"

#ifdef _MSC_VER
#include <Windows.h>
#elif defined(__linux__) // Linux
#include <signal.h>
#endif

namespace DevLib {
	namespace Routine
	{
#ifdef _MSC_VER

		BOOL WINAPI CtrlHandler(DWORD fdwCtrlType)
		{
			switch (fdwCtrlType)
			{
				// Handle the CTRL-C signal.
			case CTRL_C_EVENT:
			case CTRL_CLOSE_EVENT:
			case CTRL_BREAK_EVENT:
			case CTRL_LOGOFF_EVENT:
			case CTRL_SHUTDOWN_EVENT:

				CProcess::Exit();

				return TRUE;

			default:
				return FALSE;
			}
		}

#elif defined(__linux__) // Linux
		void CtrlHandler(int sig)
		{
			CProcess::Exit();
		}
#endif

		const std::string _routine_multicast_discovery_ip_ = "239.42.254.254";
		constexpr uint16_t _routine_multicast_discovery_port_ = 824;


		size_t CProcess::GetThreadTotalCount()
		{
			return CThread::GetThreadList().size();
		}

		std::list<std::string > CProcess::GetThreadInfo()
		{
			std::list<std::string> threadInfo = {};
			const auto list = CThread::GetThreadList();
			// for (const auto data : list)
			// {
			// 	threadInfo.push_back(data.GetObjectName());
			// }
			return threadInfo;
		}

		bool CProcess::PushNotify(const std::shared_ptr<CAbstractCaller>& notify)
		{
			Instance().m_queueNotify.Lock();
			Instance().m_queueNotify->push(notify);
			Instance().m_queueNotify.UnLock();
			return Instance().m_eNotify.SetEvent();
		}

		void CProcess::WatchdogNotify()
		{
			while (Instance().m_threadNotify.IsRunThread())
			{
				if (Instance().m_eNotify.WaitForEvent(1000))
				{
					Instance().m_queueNotify.Lock();
					size_t nQueue = Instance().m_queueNotify->size();
					Instance().m_queueNotify.UnLock();

					while (nQueue--)
					{
						Instance().m_queueNotify.Lock();
						const std::shared_ptr<CAbstractCaller> pNotify = Instance().m_queueNotify->front();
						Instance().m_queueNotify->pop();
						Instance().m_queueNotify.UnLock();

						pNotify->call();
					}
				}
			}
		}

		bool CProcess::Init()
		{
			bool bRet = false;

			// Init Notify
			if (!Instance().m_eNotify.IsCreated())
			{
				bRet = Instance().m_eNotify.Create();
				Instance().m_threadNotify.StartThread(&CProcess::WatchdogNotify, this);
			}

			// Exit
			if (!Instance().m_eDestroy.IsCreated())
			{
				bRet = Instance().m_eDestroy.Create();
				if (bRet)
				{
#if defined(_MSC_VER)
					SetConsoleCtrlHandler(CtrlHandler, TRUE);
#elif defined(__linux__) // Linux
					signal(SIGINT, CtrlHandler);
#endif
				}
			}

			return bRet;
		}

		void CProcess::Exit()
		{
			(void)Instance().m_eDestroy.SetEvent();
		}

		int32_t CProcess::Run()
		{
			while (Instance().m_eDestroy.IsCreated())
			{
				if (Instance().m_eDestroy.WaitForEvent(1000))
				{
					Instance().m_threadNotify.StopThread();
					Instance().m_eNotify.Destroy();
					Instance().m_eDestroy.Destroy();
				}
			}

			return 0;
		}
	}
}