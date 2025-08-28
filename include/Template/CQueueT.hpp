#pragma once

#include <queue>
#include "../Base/CScopeLocker.hpp"
namespace DevLib::Template
{
	template<typename Type>
	class CQueueT
	{
	public:
		void push(const Type& v)
		{
			CScopeLocker lock(locker);
			queue.push(v);
		}

		void push(Type&& v)
		{
			CScopeLocker lock(locker);
			queue.push(std::forward<Type>(v));
		}

		template <class... Args>
		decltype(auto) emplace(Args&&... args)
		{
			CScopeLocker lock(locker);
			queue.emplace(std::forward<Args>(args)...);
		}

		Type& front()
		{
			CScopeLocker lock(locker);
			return queue.front();
		}

		void pop()
		{
			CScopeLocker lock(locker);
			queue.pop();
		}

		bool empty()
		{
			CScopeLocker lock(locker);
			return queue.empty();
		}

		size_t size()
		{
			CScopeLocker lock(locker);
			return queue.size();
		}

		void Lock() const
		{
			locker.Lock();
		}

		bool TryLock() const
		{
			return locker.TryLock();
		}

		void UnLock() const
		{
			locker.UnLock();
		}

		std::queue<Type>* operator->()
		{
			return &queue;
		}

		std::queue<Type>& operator*()
		{
			return queue;
		}

		std::queue<Type>& get()
		{
			return queue;
		}

	private:
		CCriticalSection locker;
		std::queue<Type> queue;

		size_t maxQueue{ static_cast<size_t>(-1) };
	};
}
