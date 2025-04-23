#pragma once

#include "../Base/DevLibTypes.hpp"

namespace DevLib {
	namespace IO {

		class CFile final
		{
		public:

			enum SEEK_FROM
			{
				begin = 0x0,
				current = 0x1,
				end = 0x2
			};

			CFile();
			~CFile();

			bool Open(const string_t& fileName, const string_t& strMode);

			void Close();

			void Flush() const;

			size_t Read(void* pBuf, size_t nCount) const;

			size_t Write(const void* pBuf, size_t nCount) const;

			string_t ReadString(size_t nLength) const;

			string_t ReadStringLine() const;

			uint64_t WriteString(const string_t& str) const;

			void SeekToEnd() const;

			void SeekToBegin() const;

			void Seek(SEEK_FROM From, int64_t offset = 0) const;

			int64_t GetPosition() const;

			string_t GetFileName();

			string_t GetFilePath();

			int64_t GetFileSize() const;

			bool IsOpenFile() const;

			file_ptr GetFILE() const { return m_filePtr; }


		protected:
			file_ptr m_filePtr{};			// File Pointer

		private:
			string_t	m_filePath;	// File Path ( Directory\Name.Extension )
			string_t	m_fileName;	// File Name ( Name )

		public:
			CFile(const CFile&) = default;
			CFile(CFile&&) = default;

			CFile& operator=(const CFile&) = default;
			CFile& operator=(CFile&&) = default;

		};

	}
}
