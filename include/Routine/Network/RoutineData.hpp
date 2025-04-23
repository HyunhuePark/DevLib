#pragma once

#include "../Serialization/CAbstractSerialization.hpp"
#include "../Serialization/CSerializer.hpp"
#include "../Serialization/CDeSerializer.hpp"

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4201) // 4201은 비표준 익명 구조체 관련 경고 코드
#elif defined(__linux__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif

namespace DevLib {
	namespace Routine
	{

#define BeginRoutineData( dataName )\
using dataName = class dataName : public DevLib::Routine::CAbstractSerialization\
{\
	public:\
	
#if defined(_MSC_VER)

#define EndRoutineData( dataName )\
	\
	EnableSmartPointer(dataName)\
	\
	static friend DevLib::Routine::CDeSerializer& operator>>(DevLib::Routine::CDeSerializer& deserializer, dataName& obj)\
	{\
		obj.DeSerialization(deserializer);\
		return deserializer;\
	}\
	\
	DevLib::Routine::CSerializer& operator >> (DevLib::Routine::CSerializer& m_serializer)\
	{\
		Serialization(m_serializer);\
		return m_serializer;\
	}\
\
};\
\

#elif defined(__linux__) // Linux

#define EndRoutineData( dataName )\
	\
	EnableSmartPointer(dataName)\
	\
	friend DevLib::Routine::CDeSerializer& operator>>(DevLib::Routine::CDeSerializer& deserializer, dataName& obj)\
	{\
		obj.DeSerialization(deserializer);\
		return deserializer;\
	}\
	\
	DevLib::Routine::CSerializer& operator >> (DevLib::Routine::CSerializer& m_serializer)\
	{\
		Serialization(m_serializer);\
		return m_serializer;\
	}\
\
};\
\

#endif


#define BeginBitField8( dataName ) \
	struct struct_##dataName : DevLib::Routine::StBitField8_t\
	{\
		virtual  uint8_t& data()\
		{\
			return toUint8_t;\
		}\
		\
		union\
		{\
			uint8_t toUint8_t;\
			struct {\

#define EndBitField8( dataName ) } ; }; } dataName;


#define BeginBitField16( dataName ) \
	struct struct_##dataName : DevLib::Routine::StBitField16_t\
	{\
		virtual  uint16_t& data() \
		{\
			return toUint16_t;\
		}\
		\
		union\
		{\
			uint16_t toUint16_t;\
			struct {\

#define EndBitField16( dataName ) } ; }; } dataName;


#define BeginBitField32( dataName ) \
	struct struct_##dataName : DevLib::Routine::StBitField32_t\
	{\
		virtual  uint32_t& data() \
		{\
			return toUint32_t;\
		}\
		\
		union\
		{\
			uint32_t toUint32_t;\
			struct {\

#define EndBitField32( dataName ) } ; }; } dataName;



#define BeginBitField64( dataName ) \
	struct struct_##dataName : DevLib::Routine::StBitField64_t\
	{\
		virtual  uint64_t& data() \
		{\
			return toUint64_t;\
		}\
		\
		union\
		{\
			uint64_t toUint64_t;\
			struct {\

#define EndBitField64( dataName ) } ; }; } dataName;



	}
}

#if defined(_MSC_VER)
#pragma warning(pop)
#elif defined(__linux__)
#pragma GCC diagnostic pop
#endif