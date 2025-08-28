#include "../../include/MultiMedia/CRTSPProxyServer.hpp"

#if defined(_MSC_VER)
#include <liveMedia.hh>
#include <BasicUsageEnvironment.hh>
#pragma comment(lib, "crypt32.lib")
#elif defined(__linux__) // Linux
#include <liveMedia.hh>
#include <BasicUsageEnvironment.hh>
#include <Groupsock.hh>
#endif


#include "../../include/IO/DevLibSocketUtility.hpp"

#include <queue>

namespace DevLib
{
	namespace MultiMedia
	{

		inline TaskScheduler* toTaskScheduler(handle_t handle) { return static_cast<TaskScheduler*>(handle); }
		inline BasicUsageEnvironment* toBasicUsageEnvironment(handle_t handle) { return static_cast<BasicUsageEnvironment*>(handle); }
		inline RTSPServer* toRTSPServer(handle_t handle) { return static_cast<RTSPServer*>(handle); }
		inline Groupsock* toGroupSock(handle_t handle) { return static_cast<Groupsock*>(handle); }
		inline H264VideoRTPSink* toH264VideoRTPSink(handle_t handle) { return static_cast<H264VideoRTPSink*>(handle); }
		inline RTCPInstance* toRTCPInstance(handle_t handle) { return static_cast<RTCPInstance*>(handle); }
		inline ServerMediaSession* toServerMediaSession(handle_t handle) { return static_cast<ServerMediaSession*>(handle); }
		inline PassiveServerMediaSubsession* toPassiveServerMediaSubSession(handle_t handle) { return static_cast<PassiveServerMediaSubsession*>(handle); }
		inline H264VideoStreamFramer* toH264VideoStreamFramer(handle_t handle) { return static_cast<H264VideoStreamFramer*>(handle); }
		inline StreamReplicator* toStreamReplicator(handle_t handle) { return static_cast<StreamReplicator*>(handle); }
		inline UserAuthenticationDatabase* toUserAuthenticationDatabase(handle_t handle) { return static_cast<UserAuthenticationDatabase*>(handle); }

		CRTSPProxyServer::CRTSPProxyServer(std::string sessionName, bool EnableAuthentication)
			: m_scheduler(BasicTaskScheduler::createNew())
			, m_environment(BasicUsageEnvironment::createNew(*toTaskScheduler(m_scheduler)))
			, m_bEnableAuthentication(EnableAuthentication)
			, m_sessionName(std::move(sessionName))
			, bFragClosed(0)
		{
			if (m_bEnableAuthentication) m_authDB = new UserAuthenticationDatabase;
		}

		CRTSPProxyServer::~CRTSPProxyServer()
		{
			Destroy();

			Medium::close(toRTSPServer(m_rtspServer));
			toBasicUsageEnvironment(m_environment)->reclaim();
			delete toTaskScheduler(m_scheduler);

			m_environment = nullptr;
			m_scheduler = nullptr;
			m_rtspServer = nullptr;
		}

		bool CRTSPProxyServer::AddResourceServer(const std::string& url, const std::string& streamName,const std::string& id, const std::string& passwd) const
		{
			bool bRet = false;

			if (m_rtspServer)
			{
				ServerMediaSession* sms
					= ProxyServerMediaSession::createNew(*toBasicUsageEnvironment(m_environment), toRTSPServer(m_rtspServer),
						url.c_str(),
						streamName.c_str(),
						streamName.length() > 0 ? passwd.c_str() : nullptr,
						id.length() > 0 ? id.c_str() : nullptr,
						0,
						0);


				toRTSPServer(m_rtspServer)->addServerMediaSession(sms);
				bRet = true;
			}

			return bRet;

		}

		bool CRTSPProxyServer::CreateRTSPProxyServer(uint16_t serverPort)
		{
			bool bRet = false;

			if (m_rtspServer == nullptr)
			{

				m_rtspServer = RTSPServer::createNew(*toBasicUsageEnvironment(m_environment), serverPort, toUserAuthenticationDatabase(m_authDB));

				if (m_rtspServer)
				{
					OutPacketBuffer::maxSize = static_cast<uint32_t>(4096 * 1024); // 4 MB


					bRet = true;
				}
			}

			return bRet;
		}

		bool CRTSPProxyServer::ServiceStart()
		{
			bool bRet = false;
			if (m_rtspServer)
			{
				// Event Loop
				bFragClosed = 0;
				bRet = m_watchDogEvent.StartThread(&CRTSPProxyServer::watchDogEvent, this);
			}

			return bRet;
		}

		void CRTSPProxyServer::Destroy()
		{
			if (IsCreated())
			{
				bFragClosed = 1;

				m_watchDogEvent.WaitForEndThread(3000);

				Medium::close(toRTSPServer(m_rtspServer));


				m_rtspServer = nullptr;
			}
		}

		bool CRTSPProxyServer::IsCreated() const
		{
			return (m_rtspServer != nullptr);
		}
		
		void CRTSPProxyServer::watchDogEvent()
		{
			toTaskScheduler(m_scheduler)->doEventLoop(&bFragClosed);
		}
	}
}