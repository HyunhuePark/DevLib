#include "../../include/MultiMedia/CVideoDecoder.hpp"

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
		CVideoDecoder::CVideoDecoder(uint32_t nGPU)
			: m_numGPU(nGPU), m_codec(nullptr), m_context(nullptr),m_hwContext(nullptr), m_frame(nullptr), m_hwframe(nullptr), m_packet(nullptr), m_sws(nullptr), m_codecType(CodecType::Unknown), m_outputType(DevLib::Graphics::ImageType::IMG_BGR), m_cvtType(ConvertType::CVT_BILINEAR)
		{
			SetFFMpegDebugLevel(DevLib::MultiMedia::FFMPEG_DEBUG_LEVEL::No);
		}

		CVideoDecoder::~CVideoDecoder()
		{
			Destroy();
		}
		bool CVideoDecoder::CreateDecoder(CodecType type, DevLib::Graphics::ImageType outputType, ConvertType cvtType)
		{
			return CreateDecoderEx(nullptr, type, outputType, cvtType);
		}

		static enum AVPixelFormat get_hw_format(AVCodecContext *ctx, const enum AVPixelFormat *pix_fmts)
		{
			for (const enum AVPixelFormat *p = pix_fmts; *p != -1; p++)
			{
				if (*p == AV_PIX_FMT_CUDA)
				{
					return AV_PIX_FMT_CUDA;
				}
			}

			return AV_PIX_FMT_NONE;
		}

		bool CVideoDecoder::CreateDecoderEx(handle_t codecParam, CodecType type, DevLib::Graphics::ImageType outputType, ConvertType cvtType)
		{
			bool bRet = false;

			if (!IsCreated())
			{
				m_codecType = type;
				m_outputType = outputType;

				const std::string codec_name = CodecTypeToString(type);

				m_codec = reinterpret_cast<handle_t>(const_cast<AVCodec *>(avcodec_find_decoder_by_name(codec_name.c_str())));

				if (m_codec)
				{
					const auto codec = static_cast<AVCodec *>(m_codec);

					m_context = avcodec_alloc_context3(codec);

					if (m_context)
					{
						const auto context = static_cast<AVCodecContext *>(m_context);
						context->pkt_timebase = {1, 30};

						if (context->codec->id == AV_CODEC_ID_H264 || context->codec->id == AV_CODEC_ID_H265)
						{
							switch (type)
							{
							case CodecType::H264_Intel:
								av_opt_set(context->priv_data, "async_depth", "1", 0);
								break;
							case CodecType::H264_AMD:
								break;
							case CodecType::H264_NVDEC:
								av_opt_set(context->priv_data, "tune", "ull", 0);
								av_opt_set(context->priv_data, "delay", "0", 0);
								break;
							case CodecType::H264_x264:
								av_opt_set(context->priv_data, "tune", "zerolatency", 0);
								break;
							case CodecType::H264_NVENC: break;
							case CodecType::H264_NVMPI: break;
							case CodecType::H264_V4L2_M2M: break;
							case CodecType::H264_OMX: break;
							case CodecType::H264_VAAPI: break;
							case CodecType::H264_Open264: break;
							case CodecType::MJPEG: break;
							case CodecType::MJPEG_Intel: break;
							case CodecType::MJPEG_AMD: break;
							case CodecType::Unknown: break;
							case CodecType::H265_x265: break;
							case CodecType::H265_DEC: break;
							case CodecType::H265_Intel: break;
							case CodecType::H265_AMF: break;
							case CodecType::H265_MF: break;
							case CodecType::H265_NVENC: break;
							case CodecType::H265_NVDEC: break;
							default:;
							}
						}

						// Opt
						if (codecParam)
						{
							avcodec_parameters_to_context(context, static_cast<AVCodecParameters *>(codecParam));
						}

						switch (type)
						{
						case DevLib::MultiMedia::CodecType::H264_Intel:
							break;
						case DevLib::MultiMedia::CodecType::H264_AMD:
							break;
						case DevLib::MultiMedia::CodecType::H264_NVENC:
							break;
						case DevLib::MultiMedia::CodecType::H264_NVDEC:
							context->get_format = get_hw_format;

							if (av_hwdevice_ctx_create(reinterpret_cast<AVBufferRef **>(&m_hwContext), AV_HWDEVICE_TYPE_CUDA, std::to_string(m_numGPU).c_str(), nullptr, 0) >= 0)
							{
								context->hw_device_ctx = av_buffer_ref(static_cast<AVBufferRef *>(m_hwContext));
								m_hwframe = av_frame_alloc();
								const auto hwframe = static_cast<AVFrame *>(m_hwframe);
								hwframe->format = AV_PIX_FMT_NV12;
							}
							else
							{
								printf("Create Failed CUDA\n");
							}

							break;
						case DevLib::MultiMedia::CodecType::H264_NVMPI:
							break;
						case DevLib::MultiMedia::CodecType::H264_x264:
							break;
						case DevLib::MultiMedia::CodecType::H264_V4L2_M2M:
							break;
						case DevLib::MultiMedia::CodecType::H264_OMX:
							break;
						case DevLib::MultiMedia::CodecType::H264_VAAPI:
							break;
						case DevLib::MultiMedia::CodecType::H264_Open264:
							break;
						case DevLib::MultiMedia::CodecType::MJPEG:
							break;
						case DevLib::MultiMedia::CodecType::MJPEG_Intel:
							break;
						case DevLib::MultiMedia::CodecType::MJPEG_AMD:
							break;

						case CodecType::H265_x265: break;
						case CodecType::H265_DEC: break;
						case CodecType::H265_Intel: break;
						case CodecType::H265_AMF: break;
						case CodecType::H265_MF: break;
						case CodecType::H265_NVENC: break;
						case CodecType::H265_NVDEC: break;
						case CodecType::Unknown: break;
						default:
							break;
						}

						if (avcodec_open2(context, codec, nullptr) == 0)
						{
							m_packet = av_packet_alloc();
							m_frame = av_frame_alloc();

							if (m_frame && m_packet)
							{
								bRet = true;
							}
							else
							{
								Destroy();
							}
						}
						else
						{
							Destroy();
						}
					}
				}
			}

			return bRet;
		}

		void CVideoDecoder::Destroy()
		{
			csDecoded.Lock();
			if (IsCreated())
			{
				StopVideoLog();

				auto context = static_cast<AVCodecContext *>(m_context);
				auto frame = static_cast<AVFrame *>(m_frame);
				auto hwframe = static_cast<AVFrame *>(m_hwframe);
				const auto sws = static_cast<SwsContext *>(m_sws);
				auto packet = static_cast<AVPacket *>(m_packet);
				auto hwContext = static_cast<AVBufferRef *>(m_hwContext);

				avcodec_free_context(&context);
				av_frame_free(&frame);
				av_frame_free(&hwframe);
				av_packet_free(&packet);
				sws_freeContext(sws);

				if (hwContext)
				{
					av_buffer_unref(&hwContext);
				}

				m_context = nullptr;
				m_frame = nullptr;
				m_hwframe = nullptr;
				m_sws = nullptr;
				m_packet = nullptr;
				m_hwContext = nullptr;

				m_imgDecoded.Destroy();
			}
			csDecoded.UnLock();
		}

		bool CVideoDecoder::IsCreated() const
		{
			bool bRet = false;
			if (m_context)
				bRet = true;

			return bRet;
		}

		int32_t CVideoDecoder::Decode(const uint8_t *pData, size_t size)
		{
			int32_t ret = -1;
			csDecoded.Lock();
			if (IsCreated())
			{
				const auto packet = static_cast<AVPacket *>(m_packet);
				const auto context = static_cast<AVCodecContext *>(m_context);
				const auto frame = static_cast<AVFrame *>(m_frame);
				const AVFrame *pFrame = frame;

				if (av_packet_make_writable(packet) == 0)
				{
					packet->data = const_cast<uint8_t *>(pData);
					packet->size = static_cast<int32_t>(size);

					if (avcodec_send_packet(context, packet) == 0)
					{
						if (avcodec_receive_frame(context, frame) == 0)
						{
							if (m_hwContext)
							{
								const auto hwFrame = static_cast<AVFrame *>(m_hwframe);
								av_hwframe_transfer_data(hwFrame, frame, 0);
								pFrame = hwFrame;
							}

							if (!m_sws)
							{
								AVPixelFormat outputPixFMT = AV_PIX_FMT_NONE;
								int CvtFlag = SWS_BICUBIC;
								switch (m_outputType)
								{
								case Graphics::ImageType::IMG_NONE:
									break;
								case Graphics::ImageType::IMG_GRAY:
									outputPixFMT = AV_PIX_FMT_GRAY8;
									break;
								case Graphics::ImageType::IMG_BGR555:
									outputPixFMT = AV_PIX_FMT_BGR555LE;
									break;
								case Graphics::ImageType::IMG_BGR565:
									outputPixFMT = AV_PIX_FMT_BGR565LE;
									break;
								case Graphics::ImageType::IMG_BGR:
									outputPixFMT = AV_PIX_FMT_BGR24;
									break;
								case Graphics::ImageType::IMG_RGB:
									outputPixFMT = AV_PIX_FMT_RGB24;
									break;
								case Graphics::ImageType::IMG_YCbCR:
									outputPixFMT = AV_PIX_FMT_YUV444P;
									break;
								case Graphics::ImageType::IMG_HSV:
									break;
								case Graphics::ImageType::IMG_BGRA:
									outputPixFMT = AV_PIX_FMT_BGRA;
									break;
								case Graphics::ImageType::IMG_ABGR:
									outputPixFMT = AV_PIX_FMT_ABGR;
									break;
								case Graphics::ImageType::IMG_RGBA:
									outputPixFMT = AV_PIX_FMT_RGBA;
									break;
								case Graphics::ImageType::IMG_ARGB:
									outputPixFMT = AV_PIX_FMT_ARGB;
									break;
								case Graphics::ImageType::IMG_NV12: break;
								default: ;
								}

								switch (m_cvtType)
								{
								case ConvertType::CVT_FAST_BILINEAR:
									CvtFlag = SWS_FAST_BILINEAR;
									break;
								case ConvertType::CVT_BILINEAR:
									CvtFlag = SWS_BILINEAR;
									break;
								case ConvertType::CVT_BICUBIC:
									CvtFlag = SWS_BICUBIC;
									break;
								case ConvertType::CVT_X:
									CvtFlag = SWS_X;
									break;
								case ConvertType::CVT_POINT:
									CvtFlag = SWS_POINT;
									break;
								case ConvertType::CVT_AREA:
									CvtFlag = SWS_AREA;
									break;
								case ConvertType::CVT_BICUBLIN:
									CvtFlag = SWS_BICUBLIN;
									break;
								case ConvertType::CVT_GAUSS:
									CvtFlag = SWS_GAUSS;
									break;
								case ConvertType::CVT_SINC:
									CvtFlag = SWS_SINC;
									break;
								case ConvertType::CVT_LANCZOS:
									CvtFlag = SWS_LANCZOS;
									break;
								case ConvertType::CVT_SPLINE:
									CvtFlag = SWS_SPLINE;
									break;

								default:
									break;
								}

								m_imgDecoded.Create(pFrame->width, pFrame->height, m_outputType);

								if (m_hwContext && m_codecType == DevLib::MultiMedia::CodecType::H264_NVDEC)
								{
									m_sws = sws_getContext(context->width, context->height, AV_PIX_FMT_NV12, context->width, context->height, outputPixFMT, CvtFlag, nullptr, nullptr, nullptr);
								}
								else
								{
									m_sws = sws_getContext(context->width, context->height, context->pix_fmt, context->width, context->height, outputPixFMT, CvtFlag, nullptr, nullptr, nullptr);
								}
							}

							const auto sws = static_cast<SwsContext *>(m_sws);
							const int32_t lineSize = pFrame->width * m_imgDecoded.GetChannels(); // BGR = 3
							uint8_t *pDst = m_imgDecoded.GetImage();
							sws_scale(sws, pFrame->data, pFrame->linesize, 0, pFrame->height, &pDst, &lineSize);

							ret = static_cast<int32_t>(m_imgDecoded.GetSize());
							if (m_callbackDecoded)
							{
								m_callbackDecoded(m_imgDecoded);
							}

							av_frame_unref(frame);

							m_videoWriter.WriteVideo(packet->data, packet->size);
						}
					}
				}

				av_packet_unref(packet);
			}
			csDecoded.UnLock();
			return ret;
		}

		Graphics::CImage &CVideoDecoder::GetDecodedImage()
		{
			return m_imgDecoded;
		}

		handle_t CVideoDecoder::GetCodecContext() const
		{
			return m_context;
		}

		bool CVideoDecoder::StartVideoLog(const std::string &filename, int32_t width, int32_t height, int32_t fps)
		{
			bool bRet = false;
			if (IsCreated())
			{
				const auto context = static_cast<AVCodecContext *>(m_context);

				int32_t _fps = context->pkt_timebase.den;

				if (_fps < context->framerate.num)
				{
					_fps = context->framerate.num;
				}

				if (0 < fps)
				{
					_fps = fps;
				}

				int32_t _width = context->width;
				int32_t _height = context->height;

				if(_width <= 0 || _height <= 0 )
				{
					_width = width;
					_height = height;
				}

				bRet = m_videoWriter.CreateVideoFile(filename, _width, _height, _fps, CodecTypeToMediaType(m_codecType));
			}

			return bRet;
		}

		std::string CVideoDecoder::GetVideoFileName()
		{
			return m_videoWriter.GetVideoFileName();
		}

		uint32_t CVideoDecoder::GetVideoFrameCount() const
		{
			return m_videoWriter.GetVideoFrameCount();
		}

		void CVideoDecoder::StopVideoLog()
		{
			m_videoWriter.DestroyVideoFile();
		}

		bool CVideoDecoder::UseGPU(uint32_t idxGPU)
		{
			bool bRet = false;
			if (!IsCreated())
			{
				bRet = true;
				m_numGPU = idxGPU;
			}

			return bRet;
		}
	}

}