#include "../../include/MultiMedia/CVideoLoader.hpp"

#include <algorithm>

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
#include "../../include/3rd/FFmpeg/include/libavcodec/bsf.h"
#include "../../include/3rd/FFmpeg/include/libavformat/avformat.h"
#elif defined(__linux__) // Linux
#include <libavcodec/avcodec.h>

#if (LIBAVCODEC_VERSION_MAJOR < 60 && LIBAVCODEC_VERSION_MINOR < 100)
#else
#include <libavcodec/bsf.h>
#endif

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
		CVideoLoader::CVideoLoader()
			: m_context(nullptr), m_packet(nullptr), m_stream(nullptr), m_contextbsf(nullptr), m_streamIndex(0), m_frameCount(0), m_width(0), m_height(0)
		{
		}

		CVideoLoader::~CVideoLoader()
		{
			CloseVideo();
		}

		bool CVideoLoader::OpenVideo(const std::string& filename)
		{
			bool bRet = false;

			if (!IsCreated())
			{
				AVFormatContext* context = nullptr;

				if (avformat_open_input(&context, filename.c_str(), nullptr, nullptr) == 0)
				{
					if (avformat_find_stream_info(context, nullptr) == 0)
					{
						const int32_t nStreams = context->nb_streams;
						for (int32_t idx = 0; idx < nStreams; idx++)
						{
							if (context->streams[idx]->codecpar->codec_id == AV_CODEC_ID_H264)
							{
								m_packet = av_packet_alloc();

								m_streamIndex = idx;
								m_stream = context->streams[idx];
								m_context = context;

								m_width = context->streams[idx]->codecpar->width;
								m_height = context->streams[idx]->codecpar->height;

								// Check MP4

								auto ext = filename.substr(filename.size() - 4, 4);
								std::transform( ext.begin(), ext.end(), ext.begin(), ::tolower );

								if(  ext == ".mp4")
								{
									const auto bsf = av_bsf_get_by_name("h264_mp4toannexb");
									AVBSFContext* bsf_ctx = nullptr;
									av_bsf_alloc(bsf, &bsf_ctx);
									avcodec_parameters_copy(bsf_ctx->par_in, context->streams[idx]->codecpar);
									av_bsf_init(bsf_ctx);

									m_contextbsf = bsf_ctx;
								}

								bRet = true;
								break;
							}
						}
					}
				}
			}

			return bRet;
		}

		void CVideoLoader::CloseVideo()
		{
			if (IsCreated())
			{
				auto context = static_cast<AVFormatContext*> (m_context);
				auto packet = static_cast<AVPacket*>(m_packet);
				auto contextbsf = static_cast<AVBSFContext*>(m_contextbsf);

				avformat_close_input(&context);
				// avformat_free_context(context);
				av_packet_free(&packet);
				if( m_contextbsf ) av_bsf_free(&contextbsf);

				m_context = nullptr;
				m_packet = nullptr;
				m_contextbsf = nullptr;

				m_frameCount = 0;
			}
		}

		bool CVideoLoader::IsCreated() const
		{
			bool bRet = false;
			if (m_context) bRet = true;

			return bRet;
		}

		uint32_t CVideoLoader::GetImageWidth() const
		{
            return m_width;
        }

        uint32_t CVideoLoader::GetImageHeight() const
        {
            return m_height;
        }

        int64_t CVideoLoader::NextFrame()
        {
            int64_t ret = m_frameCount + 1;
			if (IsCreated())
			{
				const auto context = static_cast<AVFormatContext*> (m_context);
				const auto packet = static_cast<AVPacket*>(m_packet);

				if (av_read_frame(context, packet) == 0)
				{
					if (packet->stream_index == m_streamIndex)
					{
						if( m_contextbsf )
						{
							const auto ctx_bsf = static_cast<AVBSFContext*>(m_contextbsf);
							if( av_bsf_send_packet(ctx_bsf, packet) >= 0 )
							{
								if( av_bsf_receive_packet ( ctx_bsf, packet ) >= 0)
								{
									if (m_callbackFrameData) m_callbackFrameData(packet->data, packet->size, m_frameCount);
									m_frameCount++;
								}
							}
						}
						else
						{
							if (m_callbackFrameData) m_callbackFrameData(packet->data, packet->size, m_frameCount);
							m_frameCount++;
						}
						
						av_packet_unref(packet);
					}
					else
					{
						av_packet_unref(packet);
						NextFrame();
					}

				}
				else ret = 0;
			}
			else ret = 0;

			return ret;
        }

        handle_t CVideoLoader::GetCodecParameter() const
        {
            return static_cast<AVStream*>(m_stream)->codecpar;
        }
        int64_t CVideoLoader::GetTotalFrame() const
        {
			int64_t ret = 0;
			if (IsCreated())
			{
				const AVStream* stream = static_cast<AVStream*>(m_stream);

				if (stream->nb_frames)
				{
					ret = stream->nb_frames;
				}
				else
				{
					const auto context = static_cast<AVFormatContext*> (m_context);
					ret = static_cast<int64_t>(static_cast<float_t>(context->duration) / 1000000.F * GetFrameRate());
				}
			}

			return ret;
		}

		float_t CVideoLoader::GetFrameRate() const
		{
			float_t ret = 0.F;
			if (IsCreated())
			{
				const AVStream* stream = static_cast<AVStream*>(m_stream);

				ret = static_cast<float_t>(stream->avg_frame_rate.num) / static_cast<float_t>(stream->avg_frame_rate.den);
			}

			return ret;
		}

		int64_t CVideoLoader::SeekFrame(int64_t index)
		{
			int64_t ret = m_frameCount;
			if (IsCreated())
			{
				if (index < GetTotalFrame())
				{
					const auto context = static_cast<AVFormatContext*> (m_context);
					if (av_seek_frame(context, m_streamIndex, index, AVSEEK_FLAG_FRAME) == 0)
					{
						ret = index;
						m_frameCount = index;
					}
				}
			}
			return ret;
		}
	}
}