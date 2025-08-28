#include "../../include/MultiMedia/CVideoWriter.hpp"

#include "../../include/MultiMedia/DevLibMultimediaUtility.hpp"

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable:4819)
#endif

#ifdef __cplusplus
extern "C"
{
#endif
#if defined(_MSC_VER)
#include "../../include/3rd/FFmpeg/include/libavcodec/avcodec.h"
#include "../../include/3rd/FFmpeg/include/libavformat/avformat.h"
#elif defined(__linux__) // Linux
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#endif

#ifdef __cplusplus
}
#endif

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

namespace DevLib
{
	namespace MultiMedia
	{
		CVideoWriter::CVideoWriter()
			: m_outputContext(nullptr), m_outputStream(nullptr), m_nCount(0), m_fps(0), mediaType(MediaType::Unknown)
		{
			SetFFMpegDebugLevel(DevLib::MultiMedia::FFMPEG_DEBUG_LEVEL::No);
		}

		CVideoWriter::~CVideoWriter()
		{
			DestroyVideoFile();
		}

		bool CVideoWriter::IsCreated() const
		{
			bool bRet = false;
			if (m_outputContext) bRet = true;
			return bRet;
		}

		bool CVideoWriter::WriteVideo(const uint8_t* pNalData, const size_t nalSize)
		{
			bool bRet = false;

			m_csFile.Lock();
			if (IsCreated())
			{
				bool bFound = false;
				if (m_nCount == 0)
				{
					for (size_t i = 0; i < nalSize - 4; i++)
					{
						if (pNalData[i + 0] == 0 && pNalData[i + 1] == 0 && pNalData[i + 2] == 0 && pNalData[i + 3] == 1)
						{
							if(mediaType == MediaType::H264)
							{
								const uint8_t nalType = pNalData[i + 4] & 0x1F;

								if (nalType == 5)
								{
									bFound = true;
									break;
								}
							}

							if( mediaType == MediaType::H265)
							{
								const int nal_unit_type = (pNalData[i + 4] >> 1) & 0x3F; // 6-bit nal_unit_type

								if (nal_unit_type == 19 || nal_unit_type == 20)
								{
									bFound = true;
									break;
								}
							}
						}
					}
				}
				else
				{
					bFound = true;
				}

				if (bFound)
				{
					std::vector<size_t> vIdx;

					for (size_t i = 0; i < nalSize - 4; i++)
					{
						if (pNalData[i + 0] == 0 && pNalData[i + 1] == 0 && pNalData[i + 2] == 0 && pNalData[i + 3] == 1)
						{
							vIdx.push_back(i);
						}
					}

					if (!vIdx.empty()) vIdx.push_back(nalSize);

					AVPacket packet = {};

					for (size_t idx = 0; idx < vIdx.size() - 1; idx++)
					{


						if (mediaType == MediaType::H264)
						{
							const auto nalType = pNalData[vIdx[idx] + 4] & 0x1F;
							if (nalType == 5)
							{
								packet.flags |= AV_PKT_FLAG_KEY;
							}
						}

						if (mediaType == MediaType::H265)
						{
							const int nal_unit_type = (pNalData[vIdx[idx] + 4] >> 1) & 0x3F; // 6-bit nal_unit_type
							if (nal_unit_type == 19 || nal_unit_type == 20)
							{
								packet.flags |= AV_PKT_FLAG_KEY;
							}
						}
					}

					packet.data = const_cast<uint8_t*>(pNalData);
					packet.size = static_cast<int32_t>(nalSize);

					const auto outputContext = static_cast<AVFormatContext*>(m_outputContext);
					const auto outputStream = static_cast<AVStream*>(m_outputStream);

					packet.pts = static_cast<long long>(m_nCount * (1000.0 / m_fps));
					packet.dts = static_cast<long long>(m_nCount * (1000.0 / m_fps));
					av_packet_rescale_ts(&packet, { 1, 1000 }, outputStream->time_base);

					packet.stream_index = outputStream->index;

					// if (av_interleaved_write_frame(outputContext, &packet) == 0) 
					if (av_write_frame(outputContext, &packet) == 0)
					{
						bRet = true;
						m_nCount += 1;

						av_packet_unref(&packet);
					}
				}
			}
			m_csFile.UnLock();

			return bRet;
		}

		bool CVideoWriter::CreateVideoFile(const std::string& videoFile, int width, int height, int fps, MediaType type)
		{
			m_csFile.Lock();
			bool bRet = false;
			if (IsCreated() == false)
			{
				AVFormatContext* outputContext = nullptr;
				avformat_alloc_output_context2(&outputContext, nullptr, nullptr, videoFile.c_str());

				if (outputContext)
				{
					AVStream* streamFormat = avformat_new_stream(outputContext, nullptr);
					if (streamFormat)
					{
						streamFormat->id = outputContext->nb_streams - 1;

						AVCodecParameters codecpar = {};
						if (static_cast<int32_t>(videoFile.rfind(".avi")) > 0)
						{
							streamFormat->time_base = { 1, fps };
						}
						else
						{
							streamFormat->time_base = { 1, 90000 };
						}

						switch (type)
						{
						case MediaType::H265:
							codecpar.codec_id = AV_CODEC_ID_H265;
							break;
						case MediaType::Unknown:
						case MediaType::H264:
						default:
							codecpar.codec_id = AV_CODEC_ID_H264;
						}

						codecpar.codec_type = AVMEDIA_TYPE_VIDEO;
						codecpar.width = width;
						codecpar.height = height;

						avcodec_parameters_copy(streamFormat->codecpar, &codecpar);

						streamFormat->codecpar->codec_tag = 0;

						if (outputContext->oformat->flags & AVFMT_GLOBALHEADER)
						{
							outputContext->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
						}

						if (avio_open(&outputContext->pb, videoFile.c_str(), AVIO_FLAG_WRITE) == 0)
						{
							if (avformat_write_header(outputContext, nullptr) == 0)
							{
								bRet = true;

								m_outputStream = streamFormat;
								m_outputContext = outputContext;
								m_fps = fps;
								m_nCount = 0;
								m_videoFileName = videoFile;
								mediaType = type;
							}
						}
					}
				}
			}

			m_csFile.UnLock();
			return bRet;
		}

		std::string CVideoWriter::GetVideoFileName()
		{
			return m_videoFileName;
		}

		uint32_t CVideoWriter::GetVideoFrameCount() const
		{
			return m_nCount;
		}

		void CVideoWriter::DestroyVideoFile()
		{
			m_csFile.Lock();
			if (IsCreated())
			{
				auto outputContext = static_cast<AVFormatContext*>(m_outputContext);
				//const auto outputStream = static_cast<AVStream*>(m_outputStream);

				// end video
				av_write_trailer(outputContext);

				avio_closep(&outputContext->pb);

				//av_free(outputStream);

				avformat_close_input(&outputContext);
				avformat_free_context(outputContext);

				m_outputContext = nullptr;

				m_nCount = 0;

				m_videoFileName.clear();
			}
			m_csFile.UnLock();
		}
	}
}