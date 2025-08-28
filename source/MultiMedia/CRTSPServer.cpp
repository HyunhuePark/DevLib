#include "../../include/MultiMedia/CRTSPServer.hpp"

#if defined(_MSC_VER)
#include <liveMedia.hh>
#include <BasicUsageEnvironment.hh>
#include <GroupsockHelper.hh>
#pragma comment(lib, "crypt32.lib")
#elif defined(__linux__) // Linux
#include <liveMedia.hh>
#include <BasicUsageEnvironment.hh>
#include <Groupsock.hh>
#endif


#include "../../include/Base/CLocker.hpp"
#include "../../include/IO/DevLibSocketUtility.hpp"

#include <queue>

namespace DevLib
{
	namespace MultiMedia
	{
		class FramedSourceMem final : public FramedSource
		{
		public:
			static FramedSourceMem* createNew(UsageEnvironment& env) { return new FramedSourceMem(env); }
			FramedSourceMem(UsageEnvironment& env)
				: FramedSource(env)
				, triggerID(envir().taskScheduler().createEventTrigger(updateFrame))
			{
			}
			~FramedSourceMem() override = default;

			void PushFrame(const uint8_t* pSrc, const uint32_t size, const uint32_t sec = 0, const uint32_t usec = 0)
			{
				if (isCurrentlyAwaitingData())
				{
					if (triggerID)
					{
						frameQueue.Lock();
						frameQueue->emplace();
						frameQueue->back().first.resize(size);
						memcpy(frameQueue->back().first.data(), pSrc, frameQueue->back().first.size());
						if( sec == 0 && usec == 0)
						{
							gettimeofday(&frameQueue->back().second, nullptr);
						}
						else
						{
							frameQueue->back().second.tv_sec = sec;
							frameQueue->back().second.tv_usec = usec;
						}
						frameQueue.UnLock();

						envir().taskScheduler().triggerEvent(triggerID, this);
					}
				}
			}

		private:
			using FrameData = std::pair<std::vector<uint8_t>, timeval>;
			CLocker<std::queue<FrameData>> frameQueue;
			uint32_t triggerID;

			void doGetNextFrame() override
			{
				if (isCurrentlyAwaitingData())
				{
					frameQueue.Lock();
					if (!frameQueue->empty())
					{
						auto& frame = frameQueue->front();

						fPresentationTime = frame.second;

						if (frame.first.size() > fMaxSize)
						{
							fFrameSize = fMaxSize;
							fNumTruncatedBytes = static_cast<uint32_t>(frame.first.size() - fMaxSize);

							memmove(fTo, frame.first.data(), fFrameSize);
							frame.first.erase(frame.first.begin(), frame.first.begin() + fFrameSize);

							fDurationInMicroseconds = 0;

						}
						else
						{
							fNumTruncatedBytes = 0;
							fFrameSize = static_cast<uint32_t>(frame.first.size());
							memmove(fTo, frame.first.data(), fFrameSize);
							frameQueue->pop();
						}
					}
					else
					{
						fFrameSize = 0;
					}

					frameQueue.UnLock();

					if (fFrameSize > 0)
					{
						afterGetting(this);
					}
				}
			}

			void doStopGettingFrames() override
			{
				FramedSource::doStopGettingFrames();
			}

			static void updateFrame(void* clientData)
			{
				static_cast<FramedSourceMem*>(clientData)->doGetNextFrame();
			}
		};

		class H264MediaSubSession final : public H264VideoFileServerMediaSubsession
		{
		public:
			static H264MediaSubSession* createNew(UsageEnvironment& env, StreamReplicator* replicator)
			{
				return new H264MediaSubSession(env, replicator);
			}
		protected:
			H264MediaSubSession(UsageEnvironment& env, StreamReplicator* replicator)
				: H264VideoFileServerMediaSubsession(env, nullptr, False), m_replicator(replicator)
			{}

			FramedSource* createNewStreamSource(unsigned clientSessionId, unsigned& estBitrate) override
			{
				FramedSource* source = m_replicator->createStreamReplica();
				return H264VideoStreamDiscreteFramer::createNew(envir(), source);
			}

			RTPSink* createNewRTPSink(Groupsock* rtpGroupsock, unsigned char rtpPayloadTypeIfDynamic = 96, FramedSource* inputSource = nullptr) override
			{
				return H264VideoRTPSink::createNew(envir(), rtpGroupsock, rtpPayloadTypeIfDynamic);
			}

			StreamReplicator* m_replicator;
		};

		class H265MediaSubSession final : public H265VideoFileServerMediaSubsession
		{
		public:
			static H265MediaSubSession* createNew(UsageEnvironment& env, StreamReplicator* replicator)
			{
				return new H265MediaSubSession(env, replicator);
			}
		protected:
			H265MediaSubSession(UsageEnvironment& env, StreamReplicator* replicator)
				: H265VideoFileServerMediaSubsession(env, nullptr, False), m_replicator(replicator)
			{
			}

			FramedSource* createNewStreamSource(unsigned clientSessionId, unsigned& estBitrate) override
			{
				FramedSource* source = m_replicator->createStreamReplica();
				return H265VideoStreamDiscreteFramer::createNew(envir(), source);
			}

			RTPSink* createNewRTPSink(Groupsock* rtpGroupsock, unsigned char rtpPayloadTypeIfDynamic = 96, FramedSource* inputSource = nullptr) override
			{
				return H265VideoRTPSink::createNew(envir(), rtpGroupsock, rtpPayloadTypeIfDynamic);
			}

			StreamReplicator* m_replicator;
		};


		inline TaskScheduler* toTaskScheduler(handle_t handle) { return static_cast<TaskScheduler*>(handle); }
		inline BasicUsageEnvironment* toBasicUsageEnvironment(handle_t handle) { return static_cast<BasicUsageEnvironment*>(handle); }
		inline RTSPServer* toRTSPServer(handle_t handle) { return static_cast<RTSPServer*>(handle); }
		inline Groupsock* toGroupSock(handle_t handle) { return static_cast<Groupsock*>(handle); }
		inline RTCPInstance* toRTCPInstance(handle_t handle) { return static_cast<RTCPInstance*>(handle); }
		inline ServerMediaSession* toServerMediaSession(handle_t handle) { return static_cast<ServerMediaSession*>(handle); }
		inline PassiveServerMediaSubsession* toPassiveServerMediaSubSession(handle_t handle) { return static_cast<PassiveServerMediaSubsession*>(handle); }
		inline FramedSourceMem* toFramedSourceH264(handle_t handle) { return static_cast<FramedSourceMem*>(handle); }
		inline StreamReplicator* toStreamReplicator(handle_t handle) { return static_cast<StreamReplicator*>(handle); }
		inline UserAuthenticationDatabase* toUserAuthenticationDatabase(handle_t handle) { return static_cast<UserAuthenticationDatabase*>(handle); }

		inline H264VideoRTPSink* toH264VideoRTPSink(handle_t handle) { return static_cast<H264VideoRTPSink*>(handle); }
		inline H264VideoStreamFramer* toH264VideoStreamFramer(handle_t handle) { return static_cast<H264VideoStreamFramer*>(handle); }
		inline H264MediaSubSession* toH264MediaSubSession(handle_t handle) { return static_cast<H264MediaSubSession*>(handle); }

		inline H265VideoRTPSink* toH265VideoRTPSink(handle_t handle) { return static_cast<H265VideoRTPSink*>(handle); }
		inline H265VideoStreamFramer* toH265VideoStreamFramer(handle_t handle) { return static_cast<H265VideoStreamFramer*>(handle); }
		inline H265MediaSubSession* toH265MediaSubSession(handle_t handle) { return static_cast<H265MediaSubSession*>(handle); }

		CRTSPServer::CRTSPServer(std::string sessionName, bool EnableAuthentication)
			: m_scheduler(BasicTaskScheduler::createNew())
			, m_environment(BasicUsageEnvironment::createNew(*toTaskScheduler(m_scheduler)))
			, m_framedSource(FramedSourceMem::createNew(*toBasicUsageEnvironment(m_environment)))
			, m_bEnableAuthentication(EnableAuthentication)
			, m_sessionName(std::move(sessionName))
			, bFragClosed(0)
		{
			if (m_bEnableAuthentication) m_authDB = new UserAuthenticationDatabase;
		}

		CRTSPServer::~CRTSPServer()
		{
			Destroy();

			Medium::close(toRTSPServer(m_rtspServer));
			toBasicUsageEnvironment(m_environment)->reclaim();
			delete toTaskScheduler(m_scheduler);

			m_environment = nullptr;
			m_scheduler = nullptr;
			m_rtspServer = nullptr;
		}

		bool CRTSPServer::CreateRTSPServer(uint16_t serverPort, MediaType type, const string_t& optURL)
		{
			bool bRet = false;

			if (m_rtspServer == nullptr)
			{

				m_rtspServer = RTSPServer::createNew(*toBasicUsageEnvironment(m_environment), serverPort, toUserAuthenticationDatabase(m_authDB));

				if (m_rtspServer)
				{
					OutPacketBuffer::maxSize = static_cast<uint32_t>(8192 * 1024);

					std::string ip = "239.0.0.1";
					uint16_t rtpPort = 8888;
					bool bMulticast = false;

					if (m_callbackEnableMulticast)
					{
						bMulticast = m_callbackEnableMulticast(ip, rtpPort);
					}

					if (bMulticast)
					{
						sockaddr_storage destinationAddress{};
						destinationAddress.ss_family = AF_INET;
						reinterpret_cast<sockaddr_in&>(destinationAddress).sin_addr.s_addr = IO::SocketUtility::ipToAddr(ip);

						m_sockRTP = new Groupsock(*toBasicUsageEnvironment(m_environment), destinationAddress, Port(rtpPort), 255);
						m_sockRTCP = new Groupsock(*toBasicUsageEnvironment(m_environment), destinationAddress, Port(rtpPort + 1), 255);

						// we're a SSM source
						toGroupSock(m_sockRTP)->multicastSendOnly();
						toGroupSock(m_sockRTCP)->multicastSendOnly();

						// Buffer Size 
						m_videoSink = H264VideoRTPSink::createNew(*toBasicUsageEnvironment(m_environment), toGroupSock(m_sockRTP), 96);

						constexpr auto estimatedSessionBandwidth = static_cast<uint32_t>(8192 * 8);
						constexpr uint32_t maxCNAMElen = 100;
						uint8_t CNAME[maxCNAMElen + 1];
						gethostname(reinterpret_cast<char_t*>(CNAME), maxCNAMElen);
						CNAME[maxCNAMElen] = '\0';

						m_RTCPInstance = RTCPInstance::createNew(*toBasicUsageEnvironment(m_environment),
							toGroupSock(m_sockRTCP), estimatedSessionBandwidth,
							CNAME, toH264VideoRTPSink(m_videoSink), nullptr, True);

						m_mediaSession = ServerMediaSession::createNew(*toBasicUsageEnvironment(m_environment), optURL.c_str(), nullptr, m_sessionName.c_str(), True);
						m_mediaSubSession = PassiveServerMediaSubsession::createNew(*toH264VideoRTPSink(m_videoSink), toRTCPInstance(m_RTCPInstance));
						toServerMediaSession(m_mediaSession)->addSubsession(toPassiveServerMediaSubSession(m_mediaSubSession));
						toRTSPServer(m_rtspServer)->addServerMediaSession(toServerMediaSession(m_mediaSession));

						m_streamFramer = H264VideoStreamFramer::createNew(*toBasicUsageEnvironment(m_environment), static_cast<FramedSource*>(m_framedSource));
						toH264VideoRTPSink(m_videoSink)->startPlaying(*toH264VideoStreamFramer(m_streamFramer), nullptr, nullptr);
					}
					else
					{
						if (type == MediaType::H264)
						{
							m_streamFramer = H264VideoStreamFramer::createNew(*toBasicUsageEnvironment(m_environment), static_cast<FramedSource*>(m_framedSource));
							m_streamReplicator = StreamReplicator::createNew(*toBasicUsageEnvironment(m_environment), toH264VideoStreamFramer(m_streamFramer), false);

							m_mediaSession = ServerMediaSession::createNew(*toBasicUsageEnvironment(m_environment), optURL.c_str(), nullptr, m_sessionName.c_str());
							m_mediaSubSession = H264MediaSubSession::createNew(*toBasicUsageEnvironment(m_environment), toStreamReplicator(m_streamReplicator));
							toServerMediaSession(m_mediaSession)->addSubsession(toH264MediaSubSession(m_mediaSubSession));
							toRTSPServer(m_rtspServer)->addServerMediaSession(toServerMediaSession(m_mediaSession));
						}
						else if (type == MediaType::H265)
						{
							m_streamFramer = H265VideoStreamFramer::createNew(*toBasicUsageEnvironment(m_environment), static_cast<FramedSource*>(m_framedSource));
							m_streamReplicator = StreamReplicator::createNew(*toBasicUsageEnvironment(m_environment), toH265VideoStreamFramer(m_streamFramer), false);

							m_mediaSession = ServerMediaSession::createNew(*toBasicUsageEnvironment(m_environment), optURL.c_str(), nullptr, m_sessionName.c_str());
							m_mediaSubSession = H265MediaSubSession::createNew(*toBasicUsageEnvironment(m_environment), toStreamReplicator(m_streamReplicator));
							toServerMediaSession(m_mediaSession)->addSubsession(toH265MediaSubSession(m_mediaSubSession));
							toRTSPServer(m_rtspServer)->addServerMediaSession(toServerMediaSession(m_mediaSession));
						}
					}

					// Event Loop
					bFragClosed = 0;
					m_watchDogEvent.StartThread(&CRTSPServer::watchDogEvent, this);

					bRet = true;
				}
			}

			return bRet;
		}

		void CRTSPServer::Destroy()
		{
			if (IsCreated())
			{
				bFragClosed = 1;

				m_watchDogEvent.WaitForEndThread(3000);

				Medium::close(toRTSPServer(m_rtspServer));


				m_rtspServer = nullptr;
			}
		}

		void CRTSPServer::StreamNal(const uint8_t* pNals, const uint32_t size, const uint32_t sec, const uint32_t usec) const
		{
			toFramedSourceH264(m_framedSource)->PushFrame(pNals, size, sec, usec);
		}

		bool CRTSPServer::IsCreated() const
		{
			return (m_rtspServer != nullptr);
		}
		

		bool CRTSPServer::AddUser(const std::string& userName, const std::string& password) const
		{
			bool bRet = false;

			if (m_bEnableAuthentication)
			{
				toUserAuthenticationDatabase(m_authDB)->addUserRecord(userName.c_str(), password.c_str());
				bRet = true;
			}

			return bRet;
		}

		bool CRTSPServer::RemoveUser(const std::string& userName) const
		{
			bool bRet = false;
			if (m_bEnableAuthentication)
			{
				toUserAuthenticationDatabase(m_authDB)->removeUserRecord(userName.c_str());
				bRet = true;
			}

			return bRet;
		}

		void CRTSPServer::watchDogEvent()
		{
			toTaskScheduler(m_scheduler)->doEventLoop(&bFragClosed);
		}
	}
}