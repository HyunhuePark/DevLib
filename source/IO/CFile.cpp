#include "../../include/IO/CFile.hpp"

#if defined(_MSC_VER)
#elif defined(__linux__) // Linux
#define fopen_s(fp, fmt, mode) *(fp) = fopen((fmt), (mode))
#define _fseeki64_nolock(__stream, __off,  __whence) fseeko64(__stream, __off, __whence)
#define _ftelli64_nolock(__stream) ftello64(__stream) 
#endif

namespace DevLib {
	namespace IO {

		CFile::CFile()
			= default;

		CFile::~CFile()
		{
			Close();
		}

		bool CFile::Open(const string_t& fileName, const string_t& strMode)
		{
			bool bRet = false;
			if (IsOpenFile() == false)
			{
				fopen_s(&m_filePtr, fileName.c_str(), strMode.c_str());

				if (m_filePtr)
				{
					m_filePath = fileName;

					const size_t nPos = fileName.rfind('\\');

					if (nPos != string_t::npos)
					{
						m_fileName = fileName.substr(nPos);
					}
					else
					{
						m_fileName = fileName;
					}

					bRet = true;
				}
			}
			return bRet;
		}

		void CFile::Close()
		{
			if (m_filePtr)
			{
				(void)fclose(m_filePtr);
				m_filePtr = nullptr;
			}
		}

		void CFile::Flush() const
		{
			(void)fflush(m_filePtr);
		}

		size_t CFile::Read(void* pBuf, const size_t nCount) const
		{
			return fread(pBuf, 1, nCount, m_filePtr);
		}

		size_t CFile::Write(const void* pBuf, const size_t nCount) const
		{
			return fwrite(pBuf, nCount, 1, m_filePtr) * nCount;
		}

		string_t CFile::ReadString(const size_t nLength) const
		{
			const int64_t totalSize = GetFileSize();
			const int64_t curPos = GetPosition();

			string_t ret;
			char buff;

			for (int nReadSize = 0; nReadSize + curPos < totalSize; nReadSize++)
			{
				if (fread(&buff, 1, 1, m_filePtr) == 1)
				{
					ret += buff;
				}
				else break;

				if (ret.size() == nLength) break;
			}

			return ret;
		}

		string_t CFile::ReadStringLine() const
		{
			string_t ret;
			char buff;

			while(fread(&buff, 1, 1, m_filePtr) == 1)
			{
				if (buff == '\n') break;
				ret += buff;
			}

			return ret;
		}

		uint64_t CFile::WriteString(const string_t& str) const
		{
			return fwrite(str.c_str(), str.length(), 1, m_filePtr);
		}

		//////////////////////////////////////////////////////////////////////////
		void CFile::SeekToEnd() const
		{
			_fseeki64_nolock(m_filePtr, 0, SEEK_END);
		}

		void CFile::SeekToBegin() const
		{
			_fseeki64_nolock(m_filePtr, 0, SEEK_SET);
		}

		void CFile::Seek(const SEEK_FROM From, const int64_t offset) const
		{
			switch (From)
			{
			case begin:
				_fseeki64_nolock(m_filePtr, offset, SEEK_SET);
				break;

			case current:
				_fseeki64_nolock(m_filePtr, offset, SEEK_CUR);
				break;

			case end:
				_fseeki64_nolock(m_filePtr, offset, SEEK_END);
				break;
			}
		}

		int64_t CFile::GetPosition() const
		{
			return _ftelli64_nolock(m_filePtr);
		}

		//////////////////////////////////////////////////////////////////////////
		string_t  CFile::GetFileName()
		{
			return m_fileName;
		}

		string_t  CFile::GetFilePath()
		{
			return m_filePath;
		}


		//////////////////////////////////////////////////////////////////////////
		int64_t CFile::GetFileSize() const
		{
			const int64_t posCur = GetPosition(); //
			SeekToEnd();
			const int64_t posEnd = GetPosition(); //
			Seek(begin, posCur); //

			return posEnd;
		}

		bool CFile::IsOpenFile() const
		{
			return m_filePtr ? true : false;
		}
	}
}
