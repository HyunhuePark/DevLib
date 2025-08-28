#pragma once

#include <mutex>
#include "../Base/DevLibTypes.hpp"

namespace DevLib::Template
{
	template<typename Type>
	class CSingletonT
	{
	public:
		static Type& Instance();

		CSingletonT(CSingletonT const&) = delete;
		CSingletonT(CSingletonT&&) = delete;
		CSingletonT& operator=(CSingletonT&&) = delete;
		CSingletonT& operator=(CSingletonT const&) = delete;

	protected:
		CSingletonT()
		= default;

		~CSingletonT()
		= default;

	private:
		static std::once_flag m_flagSingle;
		static std::shared_ptr<Type> m_pInstance;

		static void Destroy();
	};

	template <typename Type>
	Type& CSingletonT<Type>::Instance()
	{
		std::call_once(m_flagSingle, []
			{
				m_pInstance.reset(new  Type());
				UNUSED(std::atexit(Destroy));
			}
		);

		return *m_pInstance;
	}

	template <typename Type>
	void CSingletonT<Type>::Destroy()
	{
		if (m_pInstance)
		{
			m_pInstance.reset();
			m_pInstance = nullptr;
		}
	}

	template <typename Type> std::shared_ptr<Type> CSingletonT<Type>::m_pInstance = nullptr;
	template <typename Type> std::once_flag CSingletonT<Type>::m_flagSingle;

}
