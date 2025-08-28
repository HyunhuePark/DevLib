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
		void SetFFMpegDebugLevel(FFMPEG_DEBUG_LEVEL level)
		{
			int32_t _log;
			switch (level)
			{
			case FFMPEG_DEBUG_LEVEL::No:
				_log = AV_LOG_QUIET;
				break;
			case FFMPEG_DEBUG_LEVEL::Panic:
				_log = AV_LOG_PANIC;
				break;
			case FFMPEG_DEBUG_LEVEL::Fatal:
				_log = AV_LOG_FATAL;
				break;
			case FFMPEG_DEBUG_LEVEL::Error:
				_log = AV_LOG_ERROR;
				break;
			case FFMPEG_DEBUG_LEVEL::Warning:
				_log = AV_LOG_WARNING;
				break;
			case FFMPEG_DEBUG_LEVEL::Standard:
				_log = AV_LOG_INFO;
				break;
			case FFMPEG_DEBUG_LEVEL::Detailed:
				_log = AV_LOG_VERBOSE;
				break;
			case FFMPEG_DEBUG_LEVEL::Debug:
				_log = AV_LOG_DEBUG;
				break;
			case FFMPEG_DEBUG_LEVEL::Trace:
				_log = AV_LOG_TRACE;
				break;
			default:
				_log = AV_LOG_QUIET;
				break;
			}
			av_log_set_level(_log);
		}
	}

} // namespace DevLib
