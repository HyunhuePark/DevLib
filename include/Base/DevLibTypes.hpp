#pragma once

#include <memory>
#include <cstdio>
#include <string>
#include <vector>

#if defined(_MSC_VER)
#define _WINSOCKAPI_ 
#include <Windows.h>
#elif defined(__linux__) // Linux
#include <ctime>
#include <semaphore.h>
#include <pthread.h>
#endif

namespace DevLib {

	using char_t = char;
	using bool_t = bool;
	using string_t = std::string;

	using int8_t = signed char;
	using int16_t = short;
	using int32_t = int;
	using int64_t = long long;

	using uint8_t = unsigned char;
	using uint16_t = unsigned short;
	using uint32_t = unsigned int;
	using uint64_t = unsigned long long;

	using size_t = std::size_t;

	using float_t = float;
	using double_t = double;

	using void_ptr = void*;
	using void_const_ptr = void const*;
	using handle_t = void_ptr;

	// SharedHandle
	using sharedHandle = std::shared_ptr<handle_t>;

	// UniqueHandle
	using uniqueHandle = std::unique_ptr<handle_t>;

	// FILE
	using file_ptr = FILE*;


#if defined(_MSC_VER)

	// Elapsed Time_t
	using elapse_t = uint64_t;

	// Event Handle
	using event_t = handle_t;

	// Serial Handle
	using serialHandle = sharedHandle;

	// shared memory Handle
	using shmHandle = sharedHandle;

	// Mutex Handle
	using mutexHandle = sharedHandle;

	// CriticalSection Handle
	using criticalSectionHandle = std::shared_ptr<CRITICAL_SECTION>;

	// CriticalSectionRW Handle
	using criticalSectionRWHandle = std::shared_ptr<SRWLOCK>;

#elif defined(__linux__) // Linux
	// Elapsed Time_t
	using elapse_t = timespec;

	// Event Handle
	using event_t = struct _event_t {
		_event_t() : sema(nullptr), bSignal(false), bInit(false) {}
		pthread_mutex_t mutex;
		pthread_cond_t	cond;
		sem_t* sema;
		std::string eventName;
		bool bSignal;
		bool bInit;
	};

	// Serial Handle
	using serialHandle = std::shared_ptr<int32_t>;

	// shared memory Handle
	using shmHandle = std::shared_ptr<int32_t>;

	// Mutex Handle
	using mutexHandle = std::shared_ptr<pthread_mutex_t>;

	// CriticalSection Handle
	using criticalSectionHandle = std::shared_ptr<pthread_spinlock_t>;

	// CriticalSectionRW Handle
	using criticalSectionRWHandle = std::shared_ptr<pthread_rwlock_t>;

#endif

	using uniqueFileHandle = std::unique_ptr<file_ptr>;

	// Type
	template <class T>
	struct IsSharedPtr : std::false_type {};

	template <class T>
	struct IsSharedPtr<std::shared_ptr<T>> : std::true_type {};

#define EnableSmartPointer( dataName )\
public:\
using WeakPtr = std::weak_ptr<dataName>; \
using SharedPtr = std::shared_ptr<dataName>; \
using ConstSharedPtr = std::shared_ptr<const dataName>; \
SharedPtr makeShared() const { return std::make_shared<dataName>(*this); }\


	/*
	 
#define EnableDebug( Object )\
	public:\
	string_t m_object##Object = {#Object};\
	void SetObjectName(const string_t& objName)\
	{\
		m_object##Object = objName;\
	}\
	\
	string_t GetObjectName()\
	{\
		return m_object##Object;\
	}\

	*/

#define UNUSED(x) ((void)(x))

}

