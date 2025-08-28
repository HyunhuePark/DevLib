#pragma once

#include "CRTSPClient.hpp"
#include "CVideoDecoder.hpp"

namespace DevLib
{
	namespace MultiMedia
	{
		class CVideoReceiver
		{
		public:
			CVideoReceiver(int nGPU = 0);
			virtual ~CVideoReceiver();

			bool CreateReceiver(const std::string& rtspURL, uint32_t msRTPTimeout = 3000, bool bUseOverTCP = false, CodecType type = CodecType::H264_Intel, DevLib::Graphics::ImageType outputType = DevLib::Graphics::ImageType::IMG_BGR, DevLib::MultiMedia::ConvertType cvtType = DevLib::MultiMedia::ConvertType::CVT_BILINEAR);
			bool IsCreated() const;

			bool WaitForReceived(uint32_t waitTimeMS) const;
			double_t GetReceivedImage(DevLib::Graphics::CImage& image);

			bool TryConnect();
			bool IsConnected() const;
			void Destroy();

			void EnableDebug( bool bEnable );
			void SetTimeout(uint32_t msTime = 3000);

			bool StartVideoLog(const std::string& filename, int32_t width = 0, int32_t height = 0, int32_t fps = 0);
			std::string GetVideoFileName();
			uint32_t GetVideoFrameCount() const;
			void StopVideoLog();

			double GetFPS() const;

			void EnableMonitoring();
			void DisableMonitoring();

			bool UseGPU(uint32_t idxGPU = 0);

			EnableCallback(Connected, const std::string& url, const std::string& sessionName)
			EnableCallback(DisConnected, const std::string& url, const std::string& sessionName)
			EnableCallback(ReceiverImage, Graphics::CImage& decImage, double_t timestamp)
			EnableCallback(Monitoring, const double& fps, const double& bitrate)

		private:
			CEvent m_waitReceivedImage;
			double m_timestampImage;

			CVideoDecoder m_decoder;
			CRTSPClient   m_client;

			void OnClientMonitoring(const double& fps, const double& bitRate) const;
			void OnConnectedRTSP(const std::string& url, const std::string& sessionName) const;
			void OnDisConnectedRTSP(const std::string& url, const std::string& sessionName) const;
			void OnRTPNals(const std::vector<uint8_t>& nal, const double& timestamp);
		};

	}
}