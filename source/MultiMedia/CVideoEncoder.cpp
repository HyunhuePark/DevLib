#include "../../include/MultiMedia/CVideoEncoder.hpp"
#include "../../include/MultiMedia/DevLibMultimediaUtility.hpp"

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4819)
#endif

#ifdef __cplusplus
extern "C"
{
#endif
#if defined(_MSC_VER)
#include "../../include/3rd/FFmpeg/include/libavcodec/avcodec.h"
#include "../../include/3rd/FFmpeg/include/libavutil/opt.h"
#include "../../include/3rd/FFmpeg/include/libswscale/swscale.h"
#include "../../include/3rd/FFmpeg/include/libavformat/avformat.h"
#elif defined(__linux__) // Linux
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
#include <libswscale/swscale.h>
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
		CVideoEncoder::CVideoEncoder(const uint32_t nGPU)
			: m_numGPU(nGPU), m_codec(nullptr), m_context(nullptr), m_contextHW(nullptr), m_frame(nullptr), m_frameHW(nullptr), m_packet(nullptr),
			  m_sws(nullptr), m_codecType(CodecType::Unknown), m_nFrame(0),
			  m_inputType(Graphics::ImageType::IMG_NONE)
		{
			SetFFMpegDebugLevel(DevLib::MultiMedia::FFMPEG_DEBUG_LEVEL::No);
			avformat_network_init();
		}

		CVideoEncoder::~CVideoEncoder()
		{
			Destroy();
			avformat_network_deinit();
		}
		bool CVideoEncoder::CreateEncoder(const int32_t width, const int32_t height, const int64_t bitRateKB, const int32_t frameRate, const int32_t gopSize, const CodecType type, const Graphics::ImageType inputType, CodecPreset preset, CodecProfile profile)
		{
			return CreateEncoderEx(width, height, bitRateKB, frameRate, gopSize, CodecTypeToString(type), inputType);
		}

		bool CVideoEncoder::CreateEncoderEx(const int32_t width, const int32_t height, const int64_t bitRateKB, const int32_t frameRate, const int32_t gopSize, const std::string &codec_name, const Graphics::ImageType inputType, const CodecPreset preset, const CodecProfile profile)
		{
			bool bRet = false;

			if (!IsCreated())
			{
				m_codec = reinterpret_cast<handle_t>(const_cast<AVCodec *>(avcodec_find_encoder_by_name(codec_name.c_str())));

				if (m_codec)
				{
					m_context = avcodec_alloc_context3(static_cast<AVCodec *>(m_codec));

					if (m_context)
					{
						m_packet = av_packet_alloc();

						if (m_packet)
						{
							const auto context = static_cast<AVCodecContext *>(m_context);

							context->bit_rate = bitRateKB * 1000;

							context->rc_buffer_size = static_cast<int32_t>(context->bit_rate) * 2;
							//context->bit_rate_tolerance = static_cast<int32_t>(static_cast<double>(context->bit_rate) * 0.1);
							context->rc_min_rate = static_cast<int32_t>(static_cast<double>(context->bit_rate) * 0.9);
							context->rc_max_rate = static_cast<int32_t>(static_cast<double>(context->bit_rate) * 1.1);

							// Resolution
							context->width = width;
							context->height = height;

							// frames per second
							context->time_base = {1, frameRate}; // Millisecond
							context->framerate = {frameRate, 1};
							context->gop_size = gopSize;

							if(context->codec->id == AV_CODEC_ID_H264)
							{
								switch (profile) {
								case CodecProfile::Baseline:
									context->profile = FF_PROFILE_H264_BASELINE;
									break;
								case CodecProfile::Main:
									context->profile = FF_PROFILE_H264_MAIN;
									break;
								case CodecProfile::High:
									context->profile = FF_PROFILE_H264_HIGH;
									break;
								case CodecProfile::High10:
									context->profile = FF_PROFILE_H264_HIGH_10;
									break;
								default:
									context->profile = FF_PROFILE_H264_MAIN;
								}

							}
							else if (context->codec->id == AV_CODEC_ID_H265)
							{
								switch (profile) {
								case CodecProfile::Baseline:
									context->profile = FF_PROFILE_HEVC_MAIN;
									break;
								case CodecProfile::Main:
									context->profile = FF_PROFILE_HEVC_MAIN;
									break;
								case CodecProfile::High:
									context->profile = FF_PROFILE_HEVC_MAIN;
									break;
								case CodecProfile::High10:
									context->profile = FF_PROFILE_HEVC_MAIN_10;
									break;
								default:
									context->profile = FF_PROFILE_HEVC_MAIN;
								}
							}

							context->flags |= AV_CODEC_FLAG_LOOP_FILTER; // def: enabled
							context->flags |= AV_CODEC_FLAG_CLOSED_GOP;
							context->slices = 0;
							context->delay = 0;
							context->thread_count = 1; // delay reduce
							// context->coder_type = FF_CODER_TYPE_VLC;
							context->max_b_frames = 0;
							context->pix_fmt = AV_PIX_FMT_YUV420P;

							av_opt_set(context->priv_data, "tune", "zerolatency", 0);
							
							if (context->codec->id == AV_CODEC_ID_H264 || context->codec->id == AV_CODEC_ID_H265)
							{
								// Preset
								switch (preset)
								{
								case CodecPreset::UltraFast: 
									av_opt_set(context, "preset", "ultrafast", 0);
									break;
								case CodecPreset::VeryFast:
									av_opt_set(context, "preset", "veryfast", 0);
									break;
								case CodecPreset::Fast: 
									av_opt_set(context, "preset", "fast", 0);
									break;
								case CodecPreset::Medium: 
									av_opt_set(context, "preset", "medium", 0);
									break;
								case CodecPreset::Slow: 
									av_opt_set(context, "preset", "slow", 0);
									break;
								case CodecPreset::VerySlow: 
									av_opt_set(context, "preset", "veryslow", 0);
									break;
								case CodecPreset::Placebo: 
									av_opt_set(context, "preset", "placebo", 0);
									break;
								default:
									av_opt_set(context, "preset", "medium", 0);
								}
					

								if (codec_name == "h264_qsv" || codec_name == "hevc_qsv")
								{
									// async : 1
									av_opt_set(context, "async_depth", "1", 0);
									context->pix_fmt = AV_PIX_FMT_NV12;

									AVBufferRef* hw_device_ctx = nullptr;

									if (av_hwdevice_ctx_create(&hw_device_ctx, AV_HWDEVICE_TYPE_QSV, nullptr, nullptr, 0) == 0)
									{
										AVBufferRef* hw_frames_ref = av_hwframe_ctx_alloc(hw_device_ctx);

										const auto frames_ctx = reinterpret_cast<AVHWFramesContext*>(hw_frames_ref->data);
										frames_ctx->format = AV_PIX_FMT_QSV;
										frames_ctx->sw_format = AV_PIX_FMT_NV12;
										frames_ctx->width = width;
										frames_ctx->height = height;
										frames_ctx->initial_pool_size = 20;

										av_hwframe_ctx_init(hw_frames_ref);

										context->hw_frames_ctx = av_buffer_ref(hw_frames_ref);

										av_buffer_unref(&hw_frames_ref);

										context->pix_fmt = AV_PIX_FMT_QSV;
										m_contextHW = hw_device_ctx;
									}
								}
								else if (codec_name == "h264_nvenc" || codec_name == "hevc_nvenc")
								{
									av_opt_set(context, "delay", "0", 0);
									av_opt_set(context, "tune", "ull", 0);
									context->pix_fmt = AV_PIX_FMT_NV12;

									AVBufferRef *hw_device_ctx = nullptr;

									if (av_hwdevice_ctx_create(&hw_device_ctx, AV_HWDEVICE_TYPE_CUDA, std::to_string(m_numGPU).c_str(), nullptr, 0) == 0)
									{
										AVBufferRef *hw_frames_ref = av_hwframe_ctx_alloc(hw_device_ctx);

										const auto frames_ctx = reinterpret_cast<AVHWFramesContext*>(hw_frames_ref->data);
										frames_ctx->format = AV_PIX_FMT_CUDA;
										frames_ctx->sw_format = AV_PIX_FMT_NV12;
										frames_ctx->width = width;
										frames_ctx->height = height;
										frames_ctx->initial_pool_size = 20;

										av_hwframe_ctx_init(hw_frames_ref);
										context->hw_frames_ctx = av_buffer_ref(hw_frames_ref);
										av_buffer_unref(&hw_frames_ref);

										context->hw_device_ctx = av_buffer_ref(hw_device_ctx);
										context->pix_fmt = AV_PIX_FMT_CUDA;
										m_contextHW = hw_device_ctx;
									}
								}
								else if (codec_name == "h264_amf" || codec_name == "hevc_amf")
								{
									context->pix_fmt = AV_PIX_FMT_NV12;
								}
								else if (codec_name == "h264_vaapi")
								{
									AVBufferRef* hw_device_ctx  = nullptr;
									
									if( av_hwdevice_ctx_create(&hw_device_ctx, AV_HWDEVICE_TYPE_VAAPI, nullptr, nullptr, 0) == 0 )
									{
										AVBufferRef *hw_frames_ref = av_hwframe_ctx_alloc(hw_device_ctx);

										const auto frames_ctx = reinterpret_cast<AVHWFramesContext*>(hw_frames_ref->data);
										frames_ctx->format = AV_PIX_FMT_VAAPI;
										frames_ctx->sw_format = AV_PIX_FMT_NV12;
										frames_ctx->width = width;
										frames_ctx->height = height;
										frames_ctx->initial_pool_size = 20;

										av_hwframe_ctx_init(hw_frames_ref);

										context->hw_frames_ctx = av_buffer_ref(hw_frames_ref);

										av_buffer_unref(&hw_frames_ref);

										context->pix_fmt = AV_PIX_FMT_VAAPI;
										m_contextHW = hw_device_ctx;
									}
								}
								else if (codec_name == "h264_nvmpi")
								{
									// av_opt_set(context->priv_data, "tune", "zerolatency", 0);
								}
								else if (codec_name == "libx264" || codec_name == "libx265")
								{
									av_opt_set(context, "tune", "zerolatency", 0);
								}
								else if( codec_name == "h264_v4l2m2m")
								{
									context->pix_fmt = AV_PIX_FMT_RGB24;
									av_opt_set(context, "tune", "zerolatency", 0);
								}
								else if (codec_name == "hevc_mf")
								{
									context->pix_fmt = AV_PIX_FMT_NV12;
									av_opt_set(context, "tune", "zerolatency", 0);
								}
							}

							const auto ret = avcodec_open2(context, static_cast<AVCodec *>(m_codec), nullptr);

							if (ret == 0)
							{
								m_frame = av_frame_alloc();

								const auto frame = static_cast<AVFrame *>(m_frame);

								if( m_contextHW ) 
								{
									const auto hwFrameContext = reinterpret_cast<AVHWFramesContext*>(context->hw_frames_ctx->data);
									frame->format = hwFrameContext->sw_format;

									m_frameHW = av_frame_alloc();
									const auto hw_frame = static_cast<AVFrame*>(m_frameHW);
									av_hwframe_get_buffer(context->hw_frames_ctx, hw_frame, 0);
								}
								else
								{
									frame->format = context->pix_fmt;
								}

								frame->width = context->width;
								frame->height = context->height;

								if (av_frame_get_buffer(frame, 0) == 0)
								{
									m_inputType = inputType;
									AVPixelFormat inputPixFMT = AV_PIX_FMT_NONE;
									switch (inputType)
									{
									case Graphics::ImageType::IMG_NONE:
										break;
									case Graphics::ImageType::IMG_GRAY:
										inputPixFMT = AV_PIX_FMT_GRAY8;
										break;
									case Graphics::ImageType::IMG_BGR555:
										inputPixFMT = AV_PIX_FMT_BGR555LE;
										break;
									case Graphics::ImageType::IMG_BGR565:
										inputPixFMT = AV_PIX_FMT_BGR565LE;
										break;
									case Graphics::ImageType::IMG_BGR:
										inputPixFMT = AV_PIX_FMT_BGR24;
										break;
									case Graphics::ImageType::IMG_RGB:
										inputPixFMT = AV_PIX_FMT_RGB24;
										break;
									case Graphics::ImageType::IMG_YCbCR:
										inputPixFMT = AV_PIX_FMT_YUV444P;
										break;
									case Graphics::ImageType::IMG_HSV:
										break;
									case Graphics::ImageType::IMG_BGRA:
										inputPixFMT = AV_PIX_FMT_BGRA;
										break;
									case Graphics::ImageType::IMG_ABGR:
										inputPixFMT = AV_PIX_FMT_ABGR;
										break;
									case Graphics::ImageType::IMG_RGBA:
										inputPixFMT = AV_PIX_FMT_RGBA;
										break;
									case Graphics::ImageType::IMG_ARGB:
										inputPixFMT = AV_PIX_FMT_ARGB;
										break;
									case Graphics::ImageType::IMG_NV12: break;
									default: ;
									}

									m_sws = sws_getContext(width, height, inputPixFMT, width, height, static_cast<AVPixelFormat>(frame->format), SWS_BICUBIC, nullptr, nullptr, nullptr);

									if (m_sws)
									{
										m_nFrame = 0;
										frame->pts = 0;

										m_codecType = CodecTypeToString(codec_name);
										bRet = true;
									}
									else
									{
										Destroy();
									}
								}
							}
							else
							{
								Destroy();
							}
						}
					}
				}
			}

			return bRet;
		}

		void CVideoEncoder::Destroy()
		{
			StopVideoLog();

			if (IsCreated())
			{
				auto context = static_cast<AVCodecContext *>(m_context);
				auto frame = static_cast<AVFrame *>(m_frame);
				auto hw_frame = static_cast<AVFrame*>(m_frameHW);
				const auto sws = static_cast<SwsContext *>(m_sws);
				auto packet = static_cast<AVPacket *>(m_packet);

				avcodec_close(context);
				avcodec_free_context(&context);
				av_frame_unref(frame);
				av_frame_free(&frame);
				av_packet_unref(packet);
				av_packet_free(&packet);
				sws_freeContext(sws);


				m_dataEncoded.clear();
				m_codec = nullptr;
				m_context = nullptr;
				m_frame = nullptr;
				m_sws = nullptr;
				m_packet = nullptr;

				if( m_contextHW ) // Using VAAPI
				{
					auto contextHW = static_cast<AVBufferRef*>(m_contextHW);
					av_buffer_unref(&contextHW);
					m_context = nullptr;

					av_frame_free(&hw_frame);
					m_frameHW = nullptr;
				}
			}
		}

		bool CVideoEncoder::IsCreated() const
		{
			bool bRet = false;
			if (m_context)
				bRet = true;

			return bRet;
		}

		int32_t CVideoEncoder::Encode(const uint8_t *pImage)
		{
			int32_t ret = -1;

			if (IsCreated())
			{
				const auto context = static_cast<AVCodecContext *>(m_context);
				const auto frame = static_cast<AVFrame *>(m_frame);
				const auto sws = static_cast<SwsContext *>(m_sws);
				const auto packet = static_cast<AVPacket *>(m_packet);

				if (av_frame_make_writable(frame) == 0)
				{
					int32_t colorSpace = 3;
					switch (m_inputType)
					{
					case Graphics::ImageType::IMG_NONE:
						break;
					case Graphics::ImageType::IMG_GRAY:
						colorSpace = 1;
						break;
					case Graphics::ImageType::IMG_BGR555:
					case Graphics::ImageType::IMG_BGR565:
						colorSpace = 2;
						break;
					case Graphics::ImageType::IMG_BGR:
					case Graphics::ImageType::IMG_RGB:
					case Graphics::ImageType::IMG_YCbCR:
						colorSpace = 3;
						break;
					case Graphics::ImageType::IMG_HSV:
						break;
					case Graphics::ImageType::IMG_BGRA:
					case Graphics::ImageType::IMG_ABGR:
					case Graphics::ImageType::IMG_RGBA:
					case Graphics::ImageType::IMG_ARGB:
						colorSpace = 4;
						break;
					case Graphics::ImageType::IMG_NV12: break;
					default: ;
					}

					const int32_t lineSize = context->width * colorSpace;
					sws_scale(sws, &pImage, &lineSize, 0, context->height, frame->data, frame->linesize);

#if defined(_MSC_VER)
					frame->pts = av_rescale_q(context->frame_num, context->framerate, context->time_base);
#else
					frame->pts = av_rescale_q(m_nFrame, context->framerate, context->time_base);
					m_nFrame++;
#endif

					if( m_contextHW ) // Using VAAPI
					{
						// CPU -> GPU
						const auto hw_frame = static_cast<AVFrame*>(m_frameHW);
						av_hwframe_transfer_data(hw_frame, frame, 0);
	
						if (avcodec_send_frame(context, hw_frame) == 0)
						{
							if (avcodec_receive_packet(context, packet) == 0)
							{
								ret = packet->size;
								m_dataEncoded.resize(packet->size);
								memcpy(m_dataEncoded.data(), packet->data, m_dataEncoded.size());

								if (m_callbackEncoded)
								{
									m_callbackEncoded(m_dataEncoded.data(), packet->size);
								}

								m_videoWriter.WriteVideo(packet->data, packet->size);

								av_packet_unref(packet);
							}
						}

					}
					else
					{
						if (avcodec_send_frame(context, frame) == 0)
						{
							if (avcodec_receive_packet(context, packet) == 0)
							{
								ret = packet->size;
								m_dataEncoded.resize(packet->size);
								memcpy(m_dataEncoded.data(), packet->data, m_dataEncoded.size());

								if (m_callbackEncoded)
								{
									m_callbackEncoded(m_dataEncoded.data(), packet->size);
								}

								m_videoWriter.WriteVideo(packet->data, packet->size);

								av_packet_unref(packet);
							}
						}
					}
				}
			}

			return ret;
		}

		int32_t CVideoEncoder::GetEncodedSize() const
		{
			return static_cast<int32_t>(m_dataEncoded.size());
		}

		uint8_t *CVideoEncoder::GetEncodedData()
		{
			return m_dataEncoded.data();
		}

		bool CVideoEncoder::StartVideoLog(const std::string &filename, const int32_t fps)
		{
			bool bRet = false;
			if (IsCreated())
			{
				const auto context = static_cast<AVCodecContext *>(m_context);

				int32_t _fps = context->framerate.num;
				if( fps > 0 ) _fps = fps;
				bRet = m_videoWriter.CreateVideoFile(filename, context->width, context->height, _fps, CodecTypeToMediaType(m_codecType));
			}

			return bRet;
		}

		std::string CVideoEncoder::GetVideoFileName()
		{
			return m_videoWriter.GetVideoFileName();
		}

		uint32_t CVideoEncoder::GetVideoFrameCount() const
		{
			return m_videoWriter.GetVideoFrameCount();
		}

		void CVideoEncoder::StopVideoLog()
		{
			m_videoWriter.DestroyVideoFile();
		}

		bool CVideoEncoder::UseGPU(const uint32_t idxGPU)
		{
			bool bRet = false;
			if (!IsCreated())
			{
				bRet = true;
				m_numGPU = idxGPU;
			}

			return bRet;
		}

		void CVideoEncoder::UpdateBitRate(const int32_t bitRate) const
		{
			const auto context = static_cast<AVCodecContext*>(m_context);

			context->bit_rate = static_cast<int64_t>(bitRate) * 1024;

			context->bit_rate_tolerance = static_cast<int32_t>(static_cast<double>(context->bit_rate) * 0.1);
			context->rc_min_rate = static_cast<int32_t>(static_cast<double>(context->bit_rate) * 0.9);
			context->rc_max_rate = static_cast<int32_t>(static_cast<double>(context->bit_rate) * 1.0);
		}

		void CVideoEncoder::UpdateFrameRate(const int32_t frameRate, const int32_t gopSize) const
		{
			const auto context = static_cast<AVCodecContext*>(m_context);

			context->time_base = { 1, frameRate }; // Millisecond
			context->framerate = { frameRate, 1 };

			if( gopSize > 0)
			{
				context->gop_size = gopSize;
			}
		}
	}
}