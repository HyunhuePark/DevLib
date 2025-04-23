#pragma once
#include "CVideoWriter.hpp"
#include "DevLibMultimediaTypes.hpp"
#include "../Base/DevLibCallback.hpp"
#include "../Graphics/CImage.hpp"

namespace DevLib
{
	namespace MultiMedia
	{
		enum class ConvertType
		{
			CVT_FAST_BILINEAR,
			CVT_BILINEAR,
			CVT_BICUBIC,
			CVT_X,
			CVT_POINT,
			CVT_AREA,
			CVT_BICUBLIN,
			CVT_GAUSS,
			CVT_SINC,
			CVT_LANCZOS,
			CVT_SPLINE
		};

		class CVideoDecoder
		{
		public:
			CVideoDecoder(uint32_t nGPU = 0);
			~CVideoDecoder();

			bool CreateDecoder(CodecType type = CodecType::H264_Intel, DevLib::Graphics::ImageType outputType = DevLib::Graphics::ImageType::IMG_BGR, ConvertType = ConvertType::CVT_BILINEAR);
			bool CreateDecoderEx(handle_t codecParam, CodecType type = CodecType::H264_Intel, DevLib::Graphics::ImageType outputType = DevLib::Graphics::ImageType::IMG_BGR, ConvertType = ConvertType::CVT_BILINEAR);

			void Destroy();
			bool IsCreated() const;

			int32_t Decode(const uint8_t* pData, size_t size);

			Graphics::CImage& GetDecodedImage();

			handle_t GetCodecContext() const;

			bool StartVideoLog(const std::string& filename, int32_t fps = 0);
			std::string GetVideoFileName();
			uint32_t GetVideoFrameCount() const;
			void StopVideoLog();

			bool UseGPU(uint32_t idxGPU = 0);

			EnableCallback(Decoded, Graphics::CImage &decImage)

		private:
			uint32_t m_numGPU;
			handle_t m_codec;
			handle_t m_context;
			handle_t m_hwContext;
			handle_t m_frame;
			handle_t m_hwframe;
			handle_t m_packet;
			handle_t m_sws;

			CVideoWriter m_videoWriter;

			CodecType m_codecType;
			Graphics::ImageType m_outputType;
			ConvertType m_cvtType;
			Graphics::CImage m_imgDecoded;
			DevLib::CCriticalSection csDecoded;
		};

	}
}
