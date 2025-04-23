#pragma once

#include "DevLibTypes.hpp"
#include <functional>

namespace DevLib {

#define EnableCallback_P0( CallbackName )\
	public :\
		using Callback##CallbackName = void(void);\
		void RegisterCallback##CallbackName(Callback##CallbackName callback)\
		{\
			m_callback##CallbackName = std::bind(callback);\
		}\
		\
		template <typename Class> void RegisterCallback##CallbackName(void(Class::* Func)(void), Class* pObject)\
		{\
			m_callback##CallbackName = std::bind(Func, pObject);\
		}\
		\
		template <typename Class> void RegisterCallback##CallbackName(void(Class::* Func)(void) const, Class* pObject)\
		{\
			m_callback##CallbackName = std::bind(Func, pObject);\
		}\
		\
		void UnRegisterCallback##CallbackName()\
		{\
			m_callback##CallbackName = nullptr;\
		}\
		\
		virtual void On##CallbackName() { } \
		\
	private:\
		std::function<Callback##CallbackName> m_callback##CallbackName;\
	public:\
	\

#define EnableCallback_P1( CallbackName, param1 )\
	public :\
		using Callback##CallbackName = void(param1);\
		void RegisterCallback##CallbackName(Callback##CallbackName callback)\
		{\
			m_callback##CallbackName = std::bind(callback, std::placeholders::_1);\
		}\
		\
		template <typename Class> void RegisterCallback##CallbackName(void(Class::* Func)(param1), Class* pObject)\
		{\
			m_callback##CallbackName = std::bind(Func, pObject, std::placeholders::_1 );\
		}\
		\
		template <typename Class> void RegisterCallback##CallbackName(void(Class::* Func)(param1) const, Class* pObject)\
		{\
			m_callback##CallbackName = std::bind(Func, pObject, std::placeholders::_1 );\
		}\
		\
		void UnRegisterCallback##CallbackName()\
		{\
			m_callback##CallbackName = nullptr;\
		}\
		\
		virtual void On##CallbackName(param1) { } \
		\
	private:\
		std::function<Callback##CallbackName> m_callback##CallbackName;\
	public:\
	\

#define EnableCallback_P2( CallbackName, param1, param2 )\
	public :\
		using Callback##CallbackName = void(param1, param2);\
		void RegisterCallback##CallbackName(Callback##CallbackName callback)\
		{\
			m_callback##CallbackName = std::bind(callback, std::placeholders::_1, std::placeholders::_2);\
		}\
		\
		template <typename Class> void RegisterCallback##CallbackName(void(Class::* Func)(param1, param2), Class* pObject)\
		{\
			m_callback##CallbackName = std::bind(Func, pObject, std::placeholders::_1, std::placeholders::_2 );\
		}\
		\
		template <typename Class> void RegisterCallback##CallbackName(void(Class::* Func)(param1, param2) const, Class* pObject)\
		{\
			m_callback##CallbackName = std::bind(Func, pObject, std::placeholders::_1, std::placeholders::_2 );\
		}\
		\
		void UnRegisterCallback##CallbackName()\
		{\
			m_callback##CallbackName = nullptr;\
		}\
		\
		virtual void On##CallbackName(param1, param2) { } \
		\
	private:\
		std::function<Callback##CallbackName> m_callback##CallbackName;\
	public:\
	\


#define EnableCallback_P3( CallbackName, param1, param2, param3 )\
	public :\
		using Callback##CallbackName = void(param1, param2, param3);\
		void RegisterCallback##CallbackName(Callback##CallbackName callback)\
		{\
			m_callback##CallbackName = std::bind(callback, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 );\
		}\
		\
		template <typename Class> void RegisterCallback##CallbackName(void(Class::* Func)(param1, param2, param3), Class* pObject)\
		{\
			m_callback##CallbackName = std::bind(Func, pObject, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 );\
		}\
		\
		template <typename Class> void RegisterCallback##CallbackName(void(Class::* Func)(param1, param2, param3) const, Class* pObject)\
		{\
			m_callback##CallbackName = std::bind(Func, pObject, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 );\
		}\
		\
		void UnRegisterCallback##CallbackName()\
		{\
			m_callback##CallbackName = nullptr;\
		}\
		\
		virtual void On##CallbackName(param1, param2, param3) { } \
		\
	private:\
		std::function<Callback##CallbackName> m_callback##CallbackName;\
	public:\
	\

#define EnableCallback_P4( CallbackName, param1, param2, param3, param4 )\
	public :\
		using Callback##CallbackName = void(param1, param2, param3, param4);\
		void RegisterCallback##CallbackName(Callback##CallbackName callback)\
		{\
			m_callback##CallbackName = std::bind(callback, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);\
		}\
		\
		template <typename Class> void RegisterCallback##CallbackName(void(Class::* Func)(param1, param2, param3, param4), Class* pObject)\
		{\
			m_callback##CallbackName = std::bind(Func, pObject, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4 );\
		}\
		\
		template <typename Class> void RegisterCallback##CallbackName(void(Class::* Func)(param1, param2, param3, param4) const, Class* pObject)\
		{\
			m_callback##CallbackName = std::bind(Func, pObject, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4 );\
		}\
		\
		void UnRegisterCallback##CallbackName()\
		{\
			m_callback##CallbackName = nullptr;\
		}\
		\
		virtual void On##CallbackName(param1, param2, param3, param4) { } \
		\
	private:\
		std::function<Callback##CallbackName> m_callback##CallbackName;\
	public:\
	\

#define EnableCallback_P5( CallbackName, param1, param2, param3, param4, param5 )\
	public :\
		using Callback##CallbackName = void(param1, param2, param3, param4, param5);\
		void RegisterCallback##CallbackName(Callback##CallbackName callback)\
		{\
			m_callback##CallbackName = std::bind(callback, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);\
		}\
		\
		template <typename Class> void RegisterCallback##CallbackName(void(Class::* Func)(param1, param2, param3, param4, param5), Class* pObject)\
		{\
			m_callback##CallbackName = std::bind(Func, pObject, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4 , std::placeholders::_5);\
		}\
		\
		template <typename Class> void RegisterCallback##CallbackName(void(Class::* Func)(param1, param2, param3, param4, param5) const, Class* pObject)\
		{\
		m_callback##CallbackName = std::bind(Func, pObject, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5); \
		}\
		\
		void UnRegisterCallback##CallbackName()\
		{\
			m_callback##CallbackName = nullptr;\
		}\
		\
		virtual void On##CallbackName(param1, param2, param3, param4, param5) { } \
		\
	private:\
		std::function<Callback##CallbackName> m_callback##CallbackName;\
	public:\
	\

#define EnableCallback_P6( CallbackName, param1, param2, param3, param4, param5, param6)\
	public :\
		using Callback##CallbackName = void(param1, param2, param3, param4, param5, param6);\
		void RegisterCallback##CallbackName(Callback##CallbackName callback)\
		{\
			m_callback##CallbackName = std::bind(callback, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6);\
		}\
		\
		template <typename Class> void RegisterCallback##CallbackName(void(Class::* Func)(param1, param2, param3, param4, param5, param6), Class* pObject)\
		{\
			m_callback##CallbackName = std::bind(Func, pObject, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4 , std::placeholders::_5, std::placeholders::_6);\
		}\
		\
		template <typename Class> void RegisterCallback##CallbackName(void(Class::* Func)(param1, param2, param3, param4, param5, param6) const, Class* pObject)\
		{\
		m_callback##CallbackName = std::bind(Func, pObject, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6); \
		}\
		\
		void UnRegisterCallback##CallbackName()\
		{\
			m_callback##CallbackName = nullptr;\
		}\
		\
		virtual void On##CallbackName(param1, param2, param3, param4, param5, param6) { } \
		\
	private:\
		std::function<Callback##CallbackName> m_callback##CallbackName;\
	public:\
	\

#define EnableCallback_P7( CallbackName, param1, param2, param3, param4, param5, param6, param7 )\
	public :\
		using Callback##CallbackName = void(param1, param2, param3, param4, param5, param6, param7);\
		void RegisterCallback##CallbackName(Callback##CallbackName callback)\
		{\
			m_callback##CallbackName = std::bind(callback, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7);\
		}\
		\
		template <typename Class> void RegisterCallback##CallbackName(void(Class::* Func)(param1, param2, param3, param4, param5, param6, param7), Class* pObject)\
		{\
			m_callback##CallbackName = std::bind(Func, pObject, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4 , std::placeholders::_5, std::placeholders::_6, std::placeholders::_7);\
		}\
		\
		template <typename Class> void RegisterCallback##CallbackName(void(Class::* Func)(param1, param2, param3, param4, param5, param6, param7) const, Class* pObject)\
		{\
			m_callback##CallbackName = std::bind(Func, pObject, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4 , std::placeholders::_5, std::placeholders::_6, std::placeholders::_7);\
		}\
		\
		void UnRegisterCallback##CallbackName()\
		{\
			m_callback##CallbackName = nullptr;\
		}\
		\
		virtual void On##CallbackName(param1, param2, param3, param4, param5, param6, param7) { } \
		\
	private:\
		std::function<Callback##CallbackName> m_callback##CallbackName;\
	public:\
	\
	// Variadic Macro
#define CONVERT_VA_CALLBACK_FUNC(_0, _1, _2, _3, _4, _5, _6, _7, N, ... ) N

#define EnableCallback( ... ) \
	CONVERT_VA_CALLBACK_FUNC(__VA_ARGS__, EnableCallback_P7, EnableCallback_P6, EnableCallback_P5, EnableCallback_P4, EnableCallback_P3, EnableCallback_P2, EnableCallback_P1, EnableCallback_P0)( __VA_ARGS__ )

	// Return Type

#define EnableReturnCallback_P0( ReturnType, CallbackName )\
	public :\
		using Callback##CallbackName = ReturnType(ReturnType);\
		void RegisterCallback##CallbackName(Callback##CallbackName callback)\
		{\
			m_callback##CallbackName = std::bind(callback);\
		}\
		\
		template <typename Class> void RegisterCallback##CallbackName(ReturnType(Class::* Func)(ReturnType), Class* pObject)\
		{\
			m_callback##CallbackName = std::bind(Func, pObject);\
		}\
		\
		template <typename Class> void RegisterCallback##CallbackName(ReturnType(Class::* Func)(ReturnType) const, Class* pObject)\
		{\
			m_callback##CallbackName = std::bind(Func, pObject);\
		}\
		\
		void UnRegisterCallback##CallbackName()\
		{\
			m_callback##CallbackName = nullptr;\
		}\
		\
		virtual ReturnType On##CallbackName() { return ReturnType{}; } \
		\
	private:\
		std::function<Callback##CallbackName> m_callback##CallbackName;\
	public:\
	\

#define EnableReturnCallback_P1( ReturnType, CallbackName, param1 )\
	public :\
		using Callback##CallbackName = ReturnType(param1);\
		void RegisterCallback##CallbackName(Callback##CallbackName callback)\
		{\
			m_callback##CallbackName = std::bind(callback, std::placeholders::_1);\
		}\
		\
		template <typename Class> void RegisterCallback##CallbackName(ReturnType(Class::* Func)(param1), Class* pObject)\
		{\
			m_callback##CallbackName = std::bind(Func, pObject, std::placeholders::_1 );\
		}\
		\
		template <typename Class> void RegisterCallback##CallbackName(ReturnType(Class::* Func)(param1) const, Class* pObject)\
		{\
			m_callback##CallbackName = std::bind(Func, pObject, std::placeholders::_1 );\
		}\
		\
		void UnRegisterCallback##CallbackName()\
		{\
			m_callback##CallbackName = nullptr;\
		}\
		\
		virtual ReturnType On##CallbackName(param1) { return ReturnType{}; } \
		\
	private:\
		std::function<Callback##CallbackName> m_callback##CallbackName;\
	public:\
	\

#define EnableReturnCallback_P2( ReturnType, CallbackName, param1, param2 )\
	public :\
		using Callback##CallbackName = ReturnType(param1, param2);\
		void RegisterCallback##CallbackName(Callback##CallbackName callback)\
		{\
			m_callback##CallbackName = std::bind(callback, std::placeholders::_1, std::placeholders::_2);\
		}\
		\
		template <typename Class> void RegisterCallback##CallbackName(ReturnType(Class::* Func)(param1, param2), Class* pObject)\
		{\
			m_callback##CallbackName = std::bind(Func, pObject, std::placeholders::_1, std::placeholders::_2 );\
		}\
		\
		template <typename Class> void RegisterCallback##CallbackName(ReturnType(Class::* Func)(param1, param2) const, Class* pObject)\
		{\
			m_callback##CallbackName = std::bind(Func, pObject, std::placeholders::_1, std::placeholders::_2 );\
		}\
		\
		void UnRegisterCallback##CallbackName()\
		{\
			m_callback##CallbackName = nullptr;\
		}\
		\
		virtual ReturnType On##CallbackName(param1, param2) { return ReturnType{}; } \
		\
	private:\
		std::function<Callback##CallbackName> m_callback##CallbackName;\
	public:\
	\


#define EnableReturnCallback_P3( ReturnType, CallbackName, param1, param2, param3 )\
	public :\
		using Callback##CallbackName = ReturnType(param1, param2, param3);\
		void RegisterCallback##CallbackName(Callback##CallbackName callback)\
		{\
			m_callback##CallbackName = std::bind(callback, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 );\
		}\
		\
		template <typename Class> void RegisterCallback##CallbackName(ReturnType(Class::* Func)(param1, param2, param3), Class* pObject)\
		{\
			m_callback##CallbackName = std::bind(Func, pObject, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 );\
		}\
		\
		template <typename Class> void RegisterCallback##CallbackName(ReturnType(Class::* Func)(param1, param2, param3) const, Class* pObject)\
		{\
			m_callback##CallbackName = std::bind(Func, pObject, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 );\
		}\
		\
		void UnRegisterCallback##CallbackName()\
		{\
			m_callback##CallbackName = nullptr;\
		}\
		\
		virtual ReturnType On##CallbackName(param1, param2, param3) { return ReturnType{}; } \
		\
	private:\
		std::function<Callback##CallbackName> m_callback##CallbackName;\
	public:\
	\

#define EnableReturnCallback_P4( ReturnType, CallbackName, param1, param2, param3, param4 )\
	public :\
		using Callback##CallbackName = ReturnType(param1, param2, param3, param4);\
		void RegisterCallback##CallbackName(Callback##CallbackName callback)\
		{\
			m_callback##CallbackName = std::bind(callback, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);\
		}\
		\
		template <typename Class> void RegisterCallback##CallbackName(ReturnType(Class::* Func)(param1, param2, param3, param4), Class* pObject)\
		{\
			m_callback##CallbackName = std::bind(Func, pObject, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4 );\
		}\
		\
		template <typename Class> void RegisterCallback##CallbackName(ReturnType(Class::* Func)(param1, param2, param3, param4) const, Class* pObject)\
		{\
			m_callback##CallbackName = std::bind(Func, pObject, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4 );\
		}\
		\
		void UnRegisterCallback##CallbackName()\
		{\
			m_callback##CallbackName = nullptr;\
		}\
		\
		virtual ReturnType On##CallbackName(param1, param2, param3, param4) { return ReturnType{}; } \
		\
	private:\
		std::function<Callback##CallbackName> m_callback##CallbackName;\
	public:\
	\

#define EnableReturnCallback_P5( ReturnType, CallbackName, param1, param2, param3, param4, param5 )\
	public :\
		using Callback##CallbackName = ReturnType(param1, param2, param3, param4, param5);\
		void RegisterCallback##CallbackName(Callback##CallbackName callback)\
		{\
			m_callback##CallbackName = std::bind(callback, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);\
		}\
		\
		template <typename Class> void RegisterCallback##CallbackName(ReturnType(Class::* Func)(param1, param2, param3, param4, param5), Class* pObject)\
		{\
			m_callback##CallbackName = std::bind(Func, pObject, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4 , std::placeholders::_5);\
		}\
		\
		template <typename Class> void RegisterCallback##CallbackName(ReturnType(Class::* Func)(param1, param2, param3, param4, param5) const, Class* pObject)\
		{\
			m_callback##CallbackName = std::bind(Func, pObject, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4 , std::placeholders::_5);\
		}\
		\
		void UnRegisterCallback##CallbackName()\
		{\
			m_callback##CallbackName = nullptr;\
		}\
		\
		virtual ReturnType On##CallbackName(param1, param2, param3, param4, param5) { return ReturnType{}; } \
		\
	private:\
		std::function<Callback##CallbackName> m_callback##CallbackName;\
	public:\
	\

#define EnableReturnCallback_P6( ReturnType, CallbackName, param1, param2, param3, param4, param5, param6)\
	public :\
		using Callback##CallbackName = ReturnType(param1, param2, param3, param4, param5, param6);\
		void RegisterCallback##CallbackName(Callback##CallbackName callback)\
		{\
			m_callback##CallbackName = std::bind(callback, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6);\
		}\
		\
		template <typename Class> void RegisterCallback##CallbackName(ReturnType(Class::* Func)(param1, param2, param3, param4, param5, param6), Class* pObject)\
		{\
			m_callback##CallbackName = std::bind(Func, pObject, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4 , std::placeholders::_5, std::placeholders::_6);\
		}\
		\
		template <typename Class> void RegisterCallback##CallbackName(ReturnType(Class::* Func)(param1, param2, param3, param4, param5, param6) const, Class* pObject)\
		{\
			m_callback##CallbackName = std::bind(Func, pObject, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4 , std::placeholders::_5, std::placeholders::_6);\
		}\
		\
		void UnRegisterCallback##CallbackName()\
		{\
			m_callback##CallbackName = nullptr;\
		}\
		\
		virtual ReturnType On##CallbackName(param1, param2, param3, param4, param5, param6) { return ReturnType{}; } \
		\
	private:\
		std::function<Callback##CallbackName> m_callback##CallbackName;\
	public:\
	\

#define EnableReturnCallback_P7( ReturnType, CallbackName, param1, param2, param3, param4, param5, param6, param7 )\
	public :\
		using Callback##CallbackName = ReturnType(param1, param2, param3, param4, param5, param6, param7);\
		void RegisterCallback##CallbackName(Callback##CallbackName callback)\
		{\
			m_callback##CallbackName = std::bind(callback, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7);\
		}\
		\
		template <typename Class> void RegisterCallback##CallbackName(ReturnType(Class::* Func)(param1, param2, param3, param4, param5, param6, param7), Class* pObject)\
		{\
			m_callback##CallbackName = std::bind(Func, pObject, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4 , std::placeholders::_5, std::placeholders::_6, std::placeholders::_7);\
		}\
		\
		template <typename Class> void RegisterCallback##CallbackName(ReturnType(Class::* Func)(param1, param2, param3, param4, param5, param6, param7) const, Class* pObject)\
		{\
			m_callback##CallbackName = std::bind(Func, pObject, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4 , std::placeholders::_5, std::placeholders::_6, std::placeholders::_7);\
		}\
		\
		void UnRegisterCallback##CallbackName()\
		{\
			m_callback##CallbackName = nullptr;\
		}\
		\
		virtual ReturnType On##CallbackName(param1, param2, param3, param4, param5, param6, param7) { return ReturnType{}; } \
		\
	private:\
		std::function<Callback##CallbackName> m_callback##CallbackName;\
	public:\
	\

	// Variadic Macro
#define CONVERT_VA_RETURN_CALLBACK_FUNC(_Return, _0, _1, _2, _3, _4, _5, _6, _7, N, ... ) N

#define EnableReturnCallback( ... ) \
	CONVERT_VA_RETURN_CALLBACK_FUNC(__VA_ARGS__, EnableReturnCallback_P7, EnableReturnCallback_P6, EnableReturnCallback_P5, EnableReturnCallback_P4, EnableReturnCallback_P3, EnableReturnCallback_P2, EnableReturnCallback_P1, EnableReturnCallback_P0)( __VA_ARGS__ )

}
