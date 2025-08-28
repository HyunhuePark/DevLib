#include "../../../include/Routine/Process/CWorker.hpp"

#include "../../../include/Routine/Process/CProcess.hpp"
#include "../../../include/Base/CElapseTimer.hpp"

namespace DevLib {
	namespace Routine
	{
		CWorker::CWorker()
			: m_bWorking(false)
		{
			RegisterCallbackWorkDone(&CWorker::OnWorkDone, this);
		}

		void CWorker::Destroy()
		{
			if (m_uptrCallerWork)
			{
				(void)WaitForDoneWork(3000);
				m_uptrCallerWork.reset();
			}
		}

		bool CWorker::Work()
		{
			bool bRet = false;
			if (m_uptrCallerWork)
			{
				if (m_eAsyncWork.IsCreated())
				{
					m_bWorking = true;
					(void)m_eAsyncWork.SetEvent();
					(void)m_eAsyncWorkDone.ResetEvent();
				}
				else
				{
					m_bWorking = true;
					CElapseTimer elapsedTimer;
					(void)m_eAsyncWorkDone.ResetEvent();
					m_uptrCallerWork->call();
					(void)m_eAsyncWorkDone.SetEvent();
					m_microWorkTime = elapsedTimer.GetElapseTime();
					m_bWorking = false;

					// WorkDone Call
					m_callbackWorkDone(std::ref(*this));

					if (m_pNextWorker) m_pNextWorker->Work();
				}

				bRet = true;
			}

			return bRet;
		}

		bool CWorker::IsWorking() const
		{
			return m_bWorking | m_eAsyncWork.IsSignaled();
		}

		bool CWorker::IsCreated() const
		{
			return m_uptrCallerWork != nullptr;
		}

		bool CWorker::WaitForDoneWork(const uint32_t waitMillisecond) const
		{
			return m_eAsyncWorkDone.WaitForEvent();
		}

		std::string CWorker::GetWorkerName() const
		{
			return m_workerName;
		}

		uint32_t CWorker::GetWorkTime() const
		{
			return m_microWorkTime;
		}

		CWorker* CWorker::GetNextWorker() const
		{
			return m_pNextWorker;
		}

		CWorker* CWorker::GetPreviousWorker() const
		{
			return m_pPreviousWork;
		}

		void CWorker::SetNextWorker(CWorker* pWorker)
		{
			m_pNextWorker = pWorker;
			pWorker->m_pPreviousWork = this;
		}

		void CWorker::WatchDogWork()
		{
			while (m_threadWork.IsRunThread())
			{
				if (m_eAsyncWork.WaitForEvent(100))
				{
					CElapseTimer elasedTimer;
					m_uptrCallerWork->call();
					(void)m_eAsyncWorkDone.SetEvent();
					m_microWorkTime = elasedTimer.GetElapseTime();
					m_bWorking = false;

					// WorkDone Call
					m_callbackWorkDone(std::ref(*this));

					if (m_pNextWorker) m_pNextWorker->Work();
				}
			}
		}
	}
}