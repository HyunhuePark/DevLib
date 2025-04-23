#pragma once

#include "../../Base/CThread.hpp"
#include "../../Base/CEvent.hpp"
#include "../../Base/DevLibCallback.hpp"

namespace DevLib {
	namespace Routine
	{
		enum class WorkerType
		{
			Asynchronous,
			Synchronous
		};

		class CWorker
		{
		public:
			CWorker();

			template<typename Func, typename... Args>
			bool CreateAsync(std::string workerName, Func&& _func, Args&&... _args);

			template<typename Func, typename... Args>
			bool CreateSync(std::string workerName, Func&& _func, Args&&... _args);

			template<typename Func, typename... Args>
			bool Create(WorkerType type, std::string workerName, Func&& _func, Args&&... _args);

			void Destroy();

			bool Work();
			bool IsWorking() const;
			bool IsCreated() const;
			bool WaitForDoneWork(uint32_t waitMillisecond = 0xFFFFFFFF) const;

			std::string GetWorkerName() const;
			uint32_t GetWorkTime() const;

			CWorker* GetNextWorker() const;
			CWorker* GetPreviousWorker() const;
			void SetNextWorker(CWorker* pWorker);

			EnableCallback(WorkDone, const CWorker& worker)

		private:
			bool m_bWorking;
			std::string m_workerName{};

			CWorker* m_pPreviousWork{};
			CWorker* m_pNextWorker{};

			uint32_t m_microWorkTime{};
			CEvent	m_eWork;
			CEvent	m_eDoneWork;
			CThread m_threadWork;

			std::unique_ptr<CAbstractCaller> m_uptrCallerWork;
			void WatchDogWork();
		};

		template <typename Func, typename ... Args>
		bool CWorker::CreateAsync(std::string workerName, Func&& _func, Args&&... _args)
		{
			return Create(WorkerType::Asynchronous, workerName, std::forward<Func>(_func), std::forward<Args>(_args)...);
		}

		template <typename Func, typename ... Args>
		bool CWorker::CreateSync(std::string workerName, Func&& _func, Args&&... _args)
		{
			return Create(WorkerType::Synchronous, workerName, std::forward<Func>(_func), std::forward<Args>(_args)...);
		}

		template <typename Func, typename ... Args>
		bool CWorker::Create(const WorkerType type, const std::string workerName, Func&& _func, Args&&... _args)
		{
			bool bRet = false;
			if (!IsCreated())
			{
				m_uptrCallerWork = std::make_unique<CVariadicCaller<Func, Args...>>(std::forward<Func>(_func), std::forward<Args>(_args)...);
				m_workerName = workerName;

				if (type == WorkerType::Asynchronous)
				{
					m_eWork.Create();
					m_eDoneWork.Create(true, false);
					bRet = m_threadWork.StartThread(&CWorker::WatchDogWork, this);
				}
			}

			return bRet;
		}
	}
}