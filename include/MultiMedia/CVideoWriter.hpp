#pragma once

#include <vector>

#include "../Base/DevLibTypes.hpp"
#include "../Base/CCriticalSection.hpp"
#include "DevLibMultimediaTypes.hpp"

namespace DevLib
{
	namespace MultiMedia
	{
		class CVideoWriter
		{
		public:
			CVideoWriter();
			~CVideoWriter();

			bool CreateVideoFile(const std::string& videoFile, int width, int height, int fps, MediaType type = MediaType::H264);
			std::string GetVideoFileName();
			uint32_t GetVideoFrameCount() const;
			void DestroyVideoFile();

			bool IsCreated() const;

			bool WriteVideo(const std::vector<uint8_t>& nalData)
			{
				return WriteVideo(nalData.data(), nalData.size());
			}

			bool WriteVideo(const uint8_t* pNalData, size_t nalSize);

		private:
			CCriticalSection m_csFile;
			std::string m_videoFileName;
			handle_t m_outputContext;
			handle_t m_outputStream;
			uint32_t m_nCount;
			int32_t  m_fps;
			MediaType mediaType;

		};

	}
}
