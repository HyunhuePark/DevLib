#pragma once
#include <string>

namespace DevLib
{
	namespace MultiMedia
	{
		enum class MediaType
		{
			Unknown,
			H264,
			H265
		};

		enum class CodecType
		{
			H264_Intel,
			H264_AMD,
			H264_NVENC,
			H264_NVDEC,
			H264_NVMPI,
			H264_x264,
			H264_V4L2_M2M,
			H264_OMX,
			H264_VAAPI,
			H264_Open264,

			H265_x265,
			H265_DEC,
			H265_Intel,
			H265_AMF,
			H265_MF,
			H265_NVENC,
			H265_NVDEC,

			MJPEG,
			MJPEG_Intel,
			MJPEG_AMD,
			Unknown
		};

		enum class CodecPreset
		{
			UltraFast,
			VeryFast,
			Fast,
			Medium,
			Slow,
			VerySlow,
			Placebo
		};

		enum class CodecProfile
		{
			Baseline,
			Main,
			High,
			High10
		};

		inline MediaType CodecTypeToMediaType(CodecType codec)
		{
			MediaType mediaType;
			switch (codec)
			{
			case CodecType::H264_AMD:
			case CodecType::H264_Intel:
			case CodecType::H264_NVENC:
			case CodecType::H264_NVDEC:
			case CodecType::H264_NVMPI:
			case CodecType::H264_x264:
			case CodecType::H264_V4L2_M2M:
			case CodecType::H264_OMX:
			case CodecType::H264_VAAPI:
			case CodecType::H264_Open264:
				mediaType = MediaType::H264;
				break;

			case CodecType::H265_x265: 
			case CodecType::H265_DEC: 
			case CodecType::H265_Intel:
			case CodecType::H265_AMF: 
			case CodecType::H265_MF: 
			case CodecType::H265_NVENC:
			case CodecType::H265_NVDEC:
				mediaType = MediaType::H265;
				break;

			case CodecType::MJPEG:
			case CodecType::MJPEG_Intel:
			case CodecType::MJPEG_AMD:
			case CodecType::Unknown:
			default:
				mediaType = MediaType::Unknown;

			}

			return mediaType;
		}

		inline std::string CodecTypeToString(CodecType codec)
		{
			std::string codec_name;
			switch (codec)
			{
			case CodecType::H264_AMD:
				codec_name = "h264_amf";
				break;
			case CodecType::H264_Intel:
				codec_name = "h264_qsv";
				break;
			case CodecType::H264_NVENC:
				codec_name = "h264_nvenc";
				break;
			case CodecType::H264_NVDEC:
				codec_name = "h264_cuvid";
				break;
			case CodecType::H264_NVMPI:
				codec_name = "h264_nvmpi";
				break;
			case CodecType::H264_x264:
				codec_name = "libx264";
				break;
			case CodecType::H264_V4L2_M2M:
				codec_name = "h264_v4l2m2m";
				break;
			case CodecType::H264_OMX:
				codec_name = "h264_omx";
				break;
			case CodecType::H264_VAAPI:
				codec_name = "h264_vaapi";
				break;
			case CodecType::H264_Open264:
				codec_name = "libopenh264";
				break;
			case CodecType::H265_x265: 
				codec_name = "libx265";
				break;
			case CodecType::H265_DEC: 
				codec_name = "hevc";
				break;
			case CodecType::H265_Intel: 
				codec_name = "hevc_qsv";
				break;
			case CodecType::H265_AMF: 
				codec_name = "hevc_amf";
				break;
			case CodecType::H265_MF: 
				codec_name = "hevc_mf";
				break;
			case CodecType::H265_NVENC:
				codec_name = "hevc_nvenc"; 
					break;
			case CodecType::H265_NVDEC:
				codec_name = "hevc_cuvid"; 
					break;
			case CodecType::MJPEG:
				codec_name = "mjpeg";
				break;
			case CodecType::MJPEG_Intel:
				codec_name = "mjpeg_cuvid";
				break;
			case CodecType::MJPEG_AMD:
				codec_name = "mjpeg_qsv";
				break;
			case CodecType::Unknown: 
				break;

			default:
				codec_name = "Unknown";
			}

			return codec_name;
		}

		inline MediaType CodecStringToMediaType(const std::string& codec_name)
		{
			if (codec_name == "h264_amf" ||
				codec_name == "h264_qsv" ||
				codec_name == "h264_nvenc" ||
				codec_name == "h264_nvmpi" ||
				codec_name == "libx264" ||
				codec_name == "h264_v4l2m2m" ||
				codec_name == "h264_vaapi" ||
				codec_name == "libopenh264"
				)
			{
				return MediaType::H264;
			}

			if (codec_name == "libx265" ||
				codec_name == "hevc_qsv" ||
				codec_name == "hevc_amf" ||
				codec_name == "hevc_mf" ||
				codec_name == "hevc_nvenc"
				)
			{
				return MediaType::H265;
			}

			return MediaType::Unknown;
		}

		inline CodecType CodecTypeToString(const std::string& codec_name)
		{
			if (codec_name == "h264_amf") return CodecType::H264_AMD;
			if (codec_name == "h264_qsv") return  CodecType::H264_Intel;
			if (codec_name == "h264_nvenc") return CodecType::H264_NVENC;
			if (codec_name == "h264_cuvid") return CodecType::H264_NVDEC;
			if (codec_name == "h264_nvmpi") return CodecType::H264_NVMPI;
			if (codec_name == "libx264") return CodecType::H264_x264;
			if (codec_name == "h264_v4l2m2m") return CodecType::H264_V4L2_M2M;
			if (codec_name == "h264_omx")  return CodecType::H264_OMX;
			if (codec_name == "h264_vaapi") return CodecType::H264_VAAPI;
			if (codec_name == "libopenh264") return CodecType::H264_Open264;
			if (codec_name == "libx265") return CodecType::H265_x265;
			if (codec_name == "hevc") return CodecType::H265_DEC;
			if (codec_name == "hevc_qsv") return CodecType::H265_Intel;
			if (codec_name == "hevc_amf") return CodecType::H265_AMF;
			if (codec_name == "hevc_mf") return CodecType::H265_MF;
			if (codec_name == "hevc_nvenc") return CodecType::H265_NVENC;
			if (codec_name == "hevc_cuvid") return CodecType::H265_NVDEC;
			if (codec_name == "mjpeg") return CodecType::MJPEG;;
			if (codec_name == "mjpeg_cuvid") return CodecType::MJPEG_Intel;
			if (codec_name == "mjpeg_qsv") return CodecType::MJPEG_AMD;
			return CodecType::Unknown;
		}

	}
}