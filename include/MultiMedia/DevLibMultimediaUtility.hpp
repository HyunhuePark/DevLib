#pragma once

namespace DevLib
{
	namespace MultiMedia
	{
		enum class FFMPEG_DEBUG_LEVEL
		{
			No,
			Panic,
			Fatal,
			Error,
			Warning,
			Standard,
			Detailed,
			Debug,
			Trace
		};

		void SetFFMpegDebugLevel(FFMPEG_DEBUG_LEVEL level = FFMPEG_DEBUG_LEVEL::No);

	}
}