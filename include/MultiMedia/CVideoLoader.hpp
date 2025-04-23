#pragma once

#include "../Base/DevLibCallback.hpp"

namespace DevLib
{
	namespace MultiMedia
	{
		class CVideoLoader
		{
		public:
			CVideoLoader();
			~CVideoLoader();

			bool OpenVideo(const std::string& filename);
			void CloseVideo();
			bool IsCreated() const;

			uint32_t GetImageWidth() const;
			uint32_t GetImageHeight() const;
			float_t GetFrameRate() const;

			int64_t GetTotalFrame() const;
			int64_t NextFrame();

			int64_t SeekFrame(int64_t index);
			int64_t BeginFrame() { return SeekFrame(0); }

			handle_t GetCodecParameter() const;

			EnableCallback(FrameData, const uint8_t* frameData, size_t frameSize, int64_t nFrame)

		private:
			handle_t m_context;
			handle_t m_packet;
			handle_t m_stream;
			handle_t m_contextbsf;
			int32_t	 m_streamIndex;
			int64_t  m_frameCount;
			uint32_t m_width;
			uint32_t m_height;
		};

	}
}
