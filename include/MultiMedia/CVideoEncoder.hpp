#pragma once
#include "DevLibMultimediaTypes.hpp"
#include "../Graphics/CImageObject.hpp"
#include "../Base/DevLibCallback.hpp"
#include "CVideoWriter.hpp"

namespace DevLib
{
	namespace MultiMedia
	{
		class CVideoEncoder
		{
		public:
			CVideoEncoder(uint32_t nGPU = 0);
			~CVideoEncoder();
			bool CreateEncoder(int32_t width, int32_t height, int64_t bitRateKB = 400, int32_t frameRate = 30, int32_t gopSize = 10, CodecType type = CodecType::H264_Intel, Graphics::ImageType inputType = Graphics::ImageType::IMG_BGR);
			bool CreateEncoderEx(int32_t width, int32_t height, int64_t bitRateKB = 400, int32_t frameRate = 30, int32_t gopSize = 10, const std::string& codec_name = "h264_qsv", Graphics::ImageType inputType = Graphics::ImageType::IMG_BGR);
			void Destroy();
			bool IsCreated() const;

			int32_t Encode(const uint8_t* pImage);

			int32_t GetEncodedSize() const;
			uint8_t* GetEncodedData();

			bool StartVideoLog(const std::string& filename, int32_t fps = 0);
			std::string GetVideoFileName();
			uint32_t GetVideoFrameCount() const;
			void StopVideoLog();

			bool UseGPU(uint32_t idxGPU = 0);
			void UpdateBitRate(int32_t bitRate) const;
			void UpdateFrameRate(int32_t frameRate, int32_t gopSize = -1) const;

			EnableCallback(Encoded, const uint8_t* pEncoded, const int32_t size)

		private:
			uint32_t m_numGPU;
			handle_t m_codec;
			handle_t m_context;
			handle_t m_contextHW;
			handle_t m_frame;
			handle_t m_frameHW;
			handle_t m_packet;
			handle_t m_sws;

			CVideoWriter m_videoWriter;

			CodecType m_codecType;

			int64_t m_nFrame;

			Graphics::ImageType m_inputType;

			std::vector<uint8_t> m_dataEncoded;

		};
	}
}
