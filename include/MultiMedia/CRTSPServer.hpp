#pragma once

#include "../Base/CThread.hpp"
#include "../Graphics/CImageObject.hpp"
#include "DevLibMultimediaTypes.hpp"
#include "../Base/DevLibCallback.hpp"

namespace DevLib
{
	namespace MultiMedia
	{
		class CRTSPServer
		{
		public:
			CRTSPServer(std::string sessionName = std::string("DevLib RTSP Streamer ( With Live555! )"), bool EnableAuthentication = false);
			virtual ~CRTSPServer();

			bool AddUser(const std::string& userName, const std::string& password) const;
			bool RemoveUser(const std::string& userName) const;

			bool CreateRTSPServer(uint16_t serverPort, MediaType type = MediaType::H264, const string_t& optURL = string_t());
			void Destroy();
			void StreamNal(const uint8_t* pNals, uint32_t size, uint32_t sec = 0, uint32_t usec = 0) const;

			bool IsCreated() const;

			EnableReturnCallback(bool, EnableMulticast, std::string& ip, uint16_t& port)

		private:
			handle_t m_scheduler{};
			handle_t m_environment{};
			handle_t m_rtspServer{};
			handle_t m_sockRTP{};
			handle_t m_sockRTCP{};
			handle_t m_videoSink{};
			handle_t m_RTCPInstance{};
			handle_t m_mediaSession{};
			handle_t m_mediaSubSession{};
			handle_t m_framedSource{};
			handle_t m_streamFramer{};
			handle_t m_streamReplicator{};
			handle_t m_authDB{};

			bool m_bEnableAuthentication;
			CThread m_watchDogEvent;

			std::string m_sessionName;

			std::atomic_char bFragClosed;
			void watchDogEvent();
		};
	}
}
