#pragma once

#include "CRTSPServer.hpp"
#include "CVideoEncoder.hpp"

namespace DevLib
{
	namespace MultiMedia
	{
		class CVideoStreamer
		{
		public:
			CVideoStreamer(std::string sessionName = std::string("DevLib RTSP Streamer ( With Live555! )"), bool EnableAuthentication = false);
			~CVideoStreamer();

			bool AddUser(const std::string& userName, const std::string& password) const;
			bool RemoveUser(const std::string& userName) const;

			bool CreateStreamer(uint16_t serverPort, int32_t width, int32_t height, int64_t bitRateKB = 400, int32_t frameRate = 30, int32_t gopSize = 15, CodecType type = CodecType::H264_Intel, Graphics::ImageType inputType = Graphics::ImageType::IMG_BGR, CodecPreset preset = CodecPreset::Medium, CodecProfile profile = CodecProfile::Main, const string_t& optURL = string_t() );
			bool CreateStreamerEx(uint16_t serverPort, int32_t width, int32_t height, int64_t bitRateKB = 400, int32_t frameRate = 30, int32_t gopSize = 15, const std::string& strType = "h264_qsv", Graphics::ImageType inputType = Graphics::ImageType::IMG_BGR, CodecPreset preset = CodecPreset::Medium, CodecProfile profile = CodecProfile::Main, const string_t& optURL = string_t() );

			bool SwitchVideoSource(int32_t width, int32_t height, int64_t bitRateKB = 400, int32_t frameRate = 30, int32_t gopSize = 15, CodecType type = CodecType::H264_Intel, Graphics::ImageType inputType = Graphics::ImageType::IMG_BGR, CodecPreset preset = CodecPreset::Medium, CodecProfile profile = CodecProfile::Main);
			bool SwitchVideoSourceEx(int32_t width, int32_t height, int64_t bitRateKB = 400, int32_t frameRate = 30, int32_t gopSize = 15, const std::string& strType = "h264_qsv", Graphics::ImageType inputType = Graphics::ImageType::IMG_BGR, CodecPreset preset = CodecPreset::Medium, CodecProfile profile = CodecProfile::Main);

			void Destroy();
			void StreamImage(const Graphics::CImageObject& src, uint32_t sec = 0, uint32_t usec = 0);

			bool IsCreated() const;

			bool StartVideoLog(const std::string &filename);
			std::string GetVideoFileName();
			uint32_t GetVideoFrameCount() const;
			void StopVideoLog();

			bool UseGPU(uint32_t idxGPU = 0);
			void UpdateBitRate(int32_t bitRate) const;
			void UpdateFrameRate(int32_t frameRate, int32_t gopSize = -1) const;

			using CallbackEnableMulticast = bool(std::string& ip, uint16_t& port);
			void RegisterCallbackEnableMulticast(CallbackEnableMulticast callback)
			{
				m_server.RegisterCallbackEnableMulticast(callback);
			}

			template <typename Class>
			void RegisterCallbackEnableMulticast(bool (Class::* Func)(std::string& ip, uint16_t& port), Class* pObject)
			{
				m_server.RegisterCallbackEnableMulticast(Func, pObject);
			}


		private:
			CRTSPServer	  m_server;
			CVideoEncoder m_encoder;
			CThread m_watchDogEvent;
		};
	}
}