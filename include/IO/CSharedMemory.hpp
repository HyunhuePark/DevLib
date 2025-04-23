#pragma once

#include "../Base/DevLibTypes.hpp"

namespace DevLib {
	namespace IO {

		class CSharedMemory
		{
		public:
			CSharedMemory();
			~CSharedMemory();

			bool Create(size_t memSize, const std::string& sharedName);

			void Destroy();

			bool IsCreated() const;

			bool WriteMemory(void_const_ptr pData, size_t size) const;

			bool ReadMemory(void_const_ptr pData, size_t size) const;

			void_ptr GetMemory() const { return m_memData; }
			size_t GetMemorySize() const { return m_sizeMem; }

		private:
			void_ptr m_memData;
			shmHandle m_handleShared;
			size_t m_sizeMem;
			std::string m_nameSharedMEM;

		public:
			CSharedMemory(const CSharedMemory&) = default;
			CSharedMemory(CSharedMemory&&) = default;

			CSharedMemory& operator=(const CSharedMemory&) = default;
			CSharedMemory& operator=(CSharedMemory&&) = default;

		};
	}
}