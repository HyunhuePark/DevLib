#pragma once

#include "../Base/CThread.hpp"
#include "../Graphics/CImageObject.hpp"

namespace DevLib
{
	namespace MultiMedia
	{
		class CRTSPProxyServer
		{
		public:
			CRTSPProxyServer(std::string sessionName = std::string("DevLib RTSP Proxy Streamer ( With Live555! )"), bool EnableAuthentication = false);
			~CRTSPProxyServer();

			bool AddResourceServer(const std::string& url, const std::string& streamName, const std::string& id = std::string(), const std::string& passwd = std::string()) const;

			bool CreateRTSPProxyServer(uint16_t serverPort);
			bool ServiceStart();
			void Destroy();

			bool IsCreated() const;

		private:
			handle_t m_scheduler{};
			handle_t m_environment{};
			handle_t m_rtspServer{};
			handle_t m_authDB{};

			bool m_bEnableAuthentication;
			CThread m_watchDogEvent;

			std::string m_sessionName;

			std::atomic_char bFragClosed;
			void watchDogEvent();
		};
	}
}
