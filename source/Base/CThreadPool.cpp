#include "../../include/Base/CThreadPool.hpp"

namespace DevLib
{
	CThreadPool::CThreadPool()
	= default;

	CThreadPool::~CThreadPool()
	= default;

	bool CThreadPool::Create(int32_t nThread, int32_t maxQueue)
	{
		bool bRet = false;
		if(vThreadPool.empty())
		{
			if (eUpdateQueue.Create())
			{
				if (eUpdateDeQueue.Create())
				{
					nIdleThread = nThread;
					vThreadPool.resize(nThread);

					for (auto& thread : vThreadPool)
					{
						thread.StartThread(&CThreadPool::OnThreadRunner, this);
					}

					if (maxQueue <= -1)
					{
						nMaxQueue = nThread * 2;
					}
					else
					{
						nMaxQueue = maxQueue;
					}


					bRet = true;
				}
				else
				{
					eUpdateQueue.Destroy();
				}
			}
		}

		return bRet;
	}

	void CThreadPool::Destroy()
	{
		if(!vThreadPool.empty())
		{
			for( auto& th : vThreadPool )
			{
				th.StopThread();
				th.WaitForEndThread(5000);
			}

			vThreadPool.resize(0);
		}

		eUpdateQueue.Destroy();
		eUpdateDeQueue.Destroy();
	}

	uint32_t CThreadPool::GetQueueSize()
	{
		const auto nSize = static_cast<uint32_t>(queCaller.size());

		return nSize;
	}

	void CThreadPool::OnThreadRunner()
	{
		const auto pThread = CThread::GetCurrentThread();
		
		while( pThread->IsRunThread() )
		{
			if(eUpdateQueue.WaitForEvent(100) )
			{
				std::unique_ptr<CAbstractCaller> pWork;

				while (TryDequeue(pWork))
				{
					if (GetQueueSize() >= GetMaxQueueSize()) (void)eUpdateDeQueue.SetEvent();

					--nIdleThread;
					pWork->call();
					++nIdleThread;
				}
			}
		}
	}

	bool CThreadPool::TryEnqueue(std::unique_ptr<CAbstractCaller> caller)
	{
		bool bRet = false;
		if (GetQueueSize() < GetMaxQueueSize())
		{
			queCaller.push(std::move(caller));

			(void)eUpdateQueue.SetEvent();

			bRet = true;
		}
		else
		{
			if (eUpdateDeQueue.WaitForEvent(1000))
			{
				queCaller.push(std::move(caller));

				(void)eUpdateQueue.SetEvent();

				bRet = true;
			}
		}

		return bRet;
	}

	bool CThreadPool::TryDequeue(std::unique_ptr<CAbstractCaller>& caller)
	{
		bool bRet = false;

		if( !queCaller.empty() )
		{
			caller = std::move(queCaller.front());
			queCaller.pop();

			bRet = true;
		}

		return bRet;
	}
}
