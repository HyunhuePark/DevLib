#pragma once

#include <future>
#include <tuple>
#include <functional>

namespace DevLib {

	class CAbstractCaller
	{
	public:
		CAbstractCaller() = default;
		virtual ~CAbstractCaller() = default;

		virtual void call() {}

		CAbstractCaller(const CAbstractCaller&) = default;
		CAbstractCaller(CAbstractCaller&&) = default;

		CAbstractCaller& operator=(const CAbstractCaller&) = default;
		CAbstractCaller& operator=(CAbstractCaller&&) = default;
	};

	template<typename Func, typename... Args>
	class CVariadicCaller final
		: public CAbstractCaller
	{
	public:
		explicit CVariadicCaller(Func&& _func, Args&&... _args) :
		m_callFunc(std::forward_as_tuple<std::decay_t<Func>, std::decay_t<Args>...>(std::forward<Func>(_func), std::forward<Args>(_args)...))
		{
		}

		~CVariadicCaller() override = default;

		void call() override
		{
			CallerVariadic<std::tuple<std::decay_t<Func>, std::decay_t<Args>...>>(std::make_index_sequence<1 + sizeof...(Args)>{});
		}

	private:
		std::tuple<std::decay_t<Func>, std::decay_t<Args>...> m_callFunc;

		template <typename Callback, size_t... Indices>
		void CallerVariadic(std::index_sequence<Indices...>)
		{
			std::invoke(std::move(std::get<Indices>(m_callFunc))...);
		}

	public:
		CVariadicCaller(const CVariadicCaller&) = default;
		CVariadicCaller(CVariadicCaller&&) = default;

		CVariadicCaller& operator=(const CVariadicCaller&) = default;
		CVariadicCaller& operator=(CVariadicCaller&&) = default;
	};
}