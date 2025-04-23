#pragma once

#include "../Base/DevLibCallback.hpp"
#include "../Base/CThread.hpp"
#include "../Base/CLocker.hpp"
#include "../Base/CEvent.hpp"
#include "../Base/CTimer.hpp"
#include "../MultiMedia/DevLibMultimediaTypes.hpp"
#include "CVideoWriter.hpp"
#include <queue>

namespace DevLib
{
	namespace MultiMedia
	{
		class CRTSPClient
		{
		public:
			CRTSPClient(uint32_t bufferSize = 100);
			~CRTSPClient();

			bool CreateRTSPClient(const std::string& rtspURL, uint32_t msRTPTimeout = 3000, DevLib::MultiMedia::MediaType type = DevLib::MultiMedia::MediaType::H264, bool bUseOverTCP = false);
			bool IsCreated() const;

			bool TryConnect();
			bool IsConnected() const;
			void Destroy();

			void EnableDebug( bool bEnable );
			void SetTimeout(uint32_t msTime = 3000);

			bool StartVideoLog(const std::string& filename, int32_t width, int32_t height, int32_t fps);
			std::string GetVideoFileName();
			uint32_t GetVideoFrameCount() const;
			void StopVideoLog();

			double GetFPS() const;

			void EnableMonitoring();
			void DisableMonitoring();
			
			EnableCallback(Connected, const std::string& url, const std::string& sessionName)
			EnableCallback(DisConnected, const std::string& url, const std::string& sessionName)
			EnableCallback(RTPData, const std::vector<uint8_t>&, const double& timestamp)
			EnableCallback(Monitoring, const double& fps, const double& bitrate)

		private:
			handle_t m_scheduler{};
			handle_t m_environment{};
			handle_t m_rtspClient{};

			CThread m_watchDogEvent;
			void watchDogEvent();

			std::string m_rtspURL;
			uint32_t m_msRTPTimeout;

			bool m_bEnableDebug = false;

			CVideoWriter m_videoWriter;

			CThread m_processNals;
			uint32_t m_maxBuffSize{100};
			using NalsPack = std::pair<double, std::vector<uint8_t>>;
			CLocker<std::queue<std::shared_ptr<NalsPack>>> m_nals;
			CEvent m_eReceived;
			void processNals();

			double m_fps{ 0 };

			CTimer m_timerMonitoring;
			void OnTimerMonitoring();
			DevLib::CLocker<uint64_t> m_totalReceived;

		protected:
			friend class DummySink;

			std::atomic_char bFragClosed;
			void OnClose(const char_t* sessionName);
			void OnConnection(const char_t* sessionName) const;

			DevLib::MultiMedia::MediaType mediaType;
			inline DevLib::MultiMedia::MediaType GetMediaType() { return mediaType; }

		};

	}
}