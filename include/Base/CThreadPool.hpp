#pragma once

#include "CEvent.hpp"
#include "CLocker.hpp"
#include "CThread.hpp"
#include "../Template/CQueueT.hpp"

namespace DevLib {

	class CThreadPool final
	{
	public:
		explicit CThreadPool();
		~CThreadPool();

		bool Create(int32_t nThread, int32_t maxQueue = -1); // Default -1 -> maxQueue = nThread *2
		void Destroy();

		template<typename Func, typename... Args>
		bool RunThread(Func&& _func, Args&&... _args);

		uint32_t GetQueueSize();
		uint32_t GetMaxQueueSize() const { return nMaxQueue; }

		uint32_t GetIdleThreadCount() { return nIdleThread; }
		uint32_t GetThreadPoolSize() const { return static_cast<uint32_t>(vThreadPool.size());  }

	private:
		CEvent eUpdateQueue;
		CEvent eUpdateDeQueue;
		std::vector<CThread> vThreadPool;
		std::atomic_uint32_t nIdleThread;
		uint32_t nMaxQueue{};
		Template::CQueueT<std::unique_ptr<CAbstractCaller>> queCaller;
		void OnThreadRunner();

		bool TryEnqueue(std::unique_ptr<CAbstractCaller> caller);
		bool TryDequeue(std::unique_ptr<CAbstractCaller >& caller);
	};

	template <typename Func, typename ... Args>
	bool CThreadPool::RunThread(Func&& _func, Args&&... _args)
	{
		return TryEnqueue(std::make_unique<CVariadicCaller<Func, Args...>>(std::forward<Func>(_func), std::forward<Args>(_args)...));
	}
}
