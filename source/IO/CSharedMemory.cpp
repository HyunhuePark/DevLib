#include "../../include/IO/CSharedMemory.hpp"

#if defined(_MSC_VER)
#pragma warning(disable : 5105)
#include <Windows.h>
#elif defined(__linux__) // Linux
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#endif

namespace DevLib {
	namespace IO {
		CSharedMemory::CSharedMemory()
#if defined(_MSC_VER)
			: m_memData(nullptr), m_handleShared(std::make_shared<void_ptr>(nullptr)), m_sizeMem(0)
#elif defined(__linux__) // Linux
			: m_memData(nullptr), m_handleShared(std::make_shared<int32_t>(-1)), m_sizeMem(0)
#endif
		{

		}

		CSharedMemory::~CSharedMemory()
		{
			if (m_handleShared.use_count() == 1)
			{
				Destroy();
			}
		}

		bool CSharedMemory::Create(const size_t memSize, const std::string& sharedName)
		{
			bool bRet = false;


			// IsOpend
#if defined(_MSC_VER)
			std::string name;
			name = std::string("Local\\_Routine_Shared_") + sharedName;

			*m_handleShared = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, name.c_str());

			// Not Open : Create
			if (!IsCreated())
			{
				*m_handleShared = CreateFileMappingA(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, static_cast<DWORD>(memSize), name.c_str());
			}

			if (IsCreated())
			{
				// �޸� ����
				m_memData = MapViewOfFile(*m_handleShared, FILE_MAP_ALL_ACCESS, 0, 0, memSize);

				if (m_memData)
				{
					// �޸� ������
					m_sizeMem = memSize;

					bRet = true;
				}
				else
				{
					CloseHandle(*m_handleShared);
					*m_handleShared = nullptr;
				}
			}
#elif defined(__linux__) // Linux

			* m_handleShared = shm_open(sharedName.c_str(), O_CREAT | O_RDWR, 0644);

			if (*m_handleShared != -1)
			{
				if (ftruncate(*m_handleShared, memSize) != -1)
				{
					m_memData = mmap(NULL, memSize, PROT_READ | PROT_WRITE, MAP_SHARED, *m_handleShared, 0);

					if (m_memData != MAP_FAILED)
					{
						bRet = true;
						m_sizeMem = memSize;
						m_nameSharedMEM = sharedName;
					}
				}
			}
#endif

			return bRet;
		}

		void CSharedMemory::Destroy()
		{
			if (IsCreated())
			{
#if defined(_MSC_VER)
				if (m_memData)
				{
					UnmapViewOfFile(m_memData);
				}

				CloseHandle(*m_handleShared);
				*m_handleShared = nullptr;

#elif defined(__linux__) // Linux
				(void)munmap(m_memData, m_sizeMem);
				//(void)shm_unlink(m_nameSharedMEM.c_str()); // Remove /dev/shm

				*m_handleShared = -1;
#endif
				m_memData = nullptr;
			}
		}

		bool CSharedMemory::IsCreated() const
		{
			return *m_handleShared ? true : false;
		}

		bool CSharedMemory::WriteMemory(const void_const_ptr pData, const size_t size) const
		{
			bool bRet = false;

			if (m_sizeMem >= size)
			{
				memcpy(GetMemory(), pData, size);
				bRet = true;
			}

			return bRet;
		}

		bool CSharedMemory::ReadMemory(const void_const_ptr pData, const size_t size) const
		{
			bool bRet = false;

			if (m_sizeMem >= size)
			{
				memcpy(const_cast<void*>(pData), GetMemory(), size);
				bRet = true;
			}

			return bRet;
		}

	}
}