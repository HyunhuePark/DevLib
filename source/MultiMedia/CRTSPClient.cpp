#include "../../include/MultiMedia/CRTSPClient.hpp"

#if defined(_MSC_VER)
#pragma comment(lib, "crypt32.lib")
#endif

#include <liveMedia.hh>
#include <BasicUsageEnvironment.hh>
#include <cfloat>

#include "../../include/Base/CElapseTimer.hpp"
#include "../../include/IO/DevLibSocketUtility.hpp"
#include "../../include/Base/CTimer.hpp"
#include "../../include/Utility/Config.hpp"

namespace DevLib
{
	namespace MultiMedia
	{
		class CRTSPClientWrapper final : public RTSPClient {
		public:
			static CRTSPClientWrapper* createNew(CRTSPClient& _parent, UsageEnvironment& env, char const* rtspURL, int verbosityLevel = 0, char const* applicationName = nullptr, portNumBits tunnelOverHTTPPortNum = 0)
			{
				return new CRTSPClientWrapper(std::forward<CRTSPClient& >(_parent), env, rtspURL, verbosityLevel, applicationName, tunnelOverHTTPPortNum);
			}

		protected:
			CRTSPClientWrapper(CRTSPClient& _parent, UsageEnvironment& env, char const* rtspURL, int verbosityLevel, char const* applicationName, portNumBits tunnelOverHTTPPortNum)
				: RTSPClient(env, rtspURL, verbosityLevel, applicationName, tunnelOverHTTPPortNum, -1),
				  parent(std::forward<CRTSPClient&>(_parent))
			{
			}

			~CRTSPClientWrapper()
			{
				const MediaSubsession* sub;
				while( (sub = iter->next()) != nullptr )
				{
					if (sub->sink != nullptr)
					{
						Medium::close(sub->sink);
						subsession->sink = nullptr;
					}
				}
				delete iter;
			}

		public:
			MediaSubsessionIterator* iter{};
			MediaSession* session{};
			MediaSubsession* subsession{};
			TaskToken streamTimerTask{};
			double duration{};
			double rtpTimeout{};
			bool bTryConnection{ false };
			bool bConnected{ false };
			bool bUseStreamOverTCP{ false };

			bool bShutdown{false};

			CRTSPClient& parent;

			EnableCallback(Connection, const char* sessionName)
			void Connection() const
			{
				if (m_callbackConnection) m_callbackConnection(session->sessionName());
			}

			EnableCallback(Closed, const char* sessionName)
			void Close() const
			{
				if (m_callbackClosed) m_callbackClosed(session->sessionName());
			}

			void SetDebug(bool bDebug)
			{
				if( bDebug) fVerbosityLevel = 1;
				else fVerbosityLevel = 0;
			}

			void SetUseStreamOverTCP(bool bUse)
			{
				bUseStreamOverTCP = bUse;
			}

			bool UsedStreamOverTCP() const
			{
				return bUseStreamOverTCP;
			}

		};

		class DummySink final : public MediaSink
		{
		public:
			static DummySink* createNew(CRTSPClient& _parent, UsageEnvironment& env,
				MediaSubsession& subsession, // identifies the kind of data that's being received
				char const* streamId = nullptr) // identifies the stream itself (optional)
			{
				return new DummySink(std::forward<CRTSPClient& >(_parent), env, subsession, streamId);
			}

			bool IsUpdateFrame()
			{
				bool bRet = false;

				if (m_eTime.GetElapseTime() > 1000000) // 1s
				{
					if (nFrameCount != nFrameCountPrev)
					{
						bRet = true;
					}
					nFrameCountPrev = nFrameCount;

					m_eTime.Reset();
				}


				return bRet;
			}

			uint64_t GetFrameCount() const
			{
				return nFrameCount;
			}

		private:

#define DUMMY_SINK_RECEIVE_BUFFER_SIZE (1024*4096)

			DummySink(CRTSPClient& _parent, UsageEnvironment& env, MediaSubsession& subsession, char const* streamId)
				: MediaSink(env), nFrameCount(0), nFrameCountPrev(-1),
				 fSubsession(subsession), fStreamId(strDup(streamId)), parent(std::forward<CRTSPClient& >(_parent))
			{
				fReceiveBuffer.resize(DUMMY_SINK_RECEIVE_BUFFER_SIZE);
				memset(fReceiveBuffer.data(), 0, DUMMY_SINK_RECEIVE_BUFFER_SIZE);
			}

			~DummySink() override
			{
				delete[] fStreamId;
			}


			// Paser H264
			// https://github.com/melchi45/rtspclient_with_opengl/blob/master/src/MediaH264MediaSink.cpp
			typedef enum nal_type
			{
				NALTYPE_Unspecified = 0,
				NALTYPE_SliceLayerWithoutPartitioning = 1,
				NALTYPE_SliceDataPartitionALayer = 2,
				NALTYPE_SliceDataPartitionBLayer = 3,
				NALTYPE_SliceDataPartitionCLayer = 4,
				NALTYPE_IDRPicture = 5,
				NALTYPE_SEI = 6,
				NALTYPE_SequenceParameterSet = 7,	// SPS
				NALTYPE_PictureParameterSet = 8,	// PPS
				NALTYPE_AccessUnitDelimiter = 9,
				NALTYPE_EndofSequence = 10,
				NALTYPE_EndofStream = 11,
				NALTYPE_FilterData = 12,
				NALTYPE_Extended = 13,
				//	NALTYPE_Unspecified = 24,
			} NALTYPE;

			static bool FindStartCode3(const unsigned char* Buf)
			{
				if (Buf[0] != 0 || Buf[1] != 0 || Buf[2] != 1) return false; // check header 0x000001
				else return true;
			}

			static bool FindStartCode4(const unsigned char* Buf)
			{
				if (Buf[0] != 0 || Buf[1] != 0 || Buf[2] != 0 || Buf[3] != 1) return false;//check header 0x00000001
				else return true;
			}

			static bool isH264iFrame(const u_int8_t* packet)
			{
				//	assert(packet);
				int pos = 0;

				// check 3 byte start code
				if (FindStartCode3(packet))
				{
					pos = 3;
				}

				if (FindStartCode3(packet))
				{
					pos = 4;
				}

				constexpr int RTPHeaderBytes = 0;

				const int fragment_type = packet[RTPHeaderBytes + pos + 0] & 0x1F;
				const int nal_type = packet[RTPHeaderBytes + pos + 1] & 0x1F;			// 5 bit 
				const int start_bit = packet[RTPHeaderBytes + pos + 1] & 0x80;		// 1 bit
				// int reference_idc = packet[RTPHeaderBytes + pos + 1] & 0x60;	// 2 bit


				if (((fragment_type == 28 || fragment_type == 29) && NALTYPE::NALTYPE_IDRPicture == nal_type && start_bit == 128) || fragment_type == 5)
				{
					return true;
				}

				return false;
			}

			static bool isH265IFrame(const u_int8_t* packet)
			{
				// assert(packet);
				int pos = 0;

				// check 3 byte start code
				if (FindStartCode3(packet))
				{
					pos = 3;
				}

				// check 4 byte start code
				if (FindStartCode4(packet))
				{
					pos = 4;
				}

				constexpr int RTPHeaderBytes = 0;

				// Extract NAL Unit Header fields
				const int nal_unit_type = (packet[RTPHeaderBytes + pos] >> 1) & 0x3F; // 6-bit nal_unit_type
				//const int nuh_layer_id = (packet[RTPHeaderBytes + pos] >> 7) & 0x3;   // 2-bit nuh_layer_id
				//const int nuh_temporal_id_plus1 = (packet[RTPHeaderBytes + pos + 1] & 0x07); // 3-bit temporal_id_plus1

				// Check if it is an I-Frame (IDR Picture)
				if (nal_unit_type == 19 || nal_unit_type == 20) // IDR_W_RADL(19), IDR_N_LP(20)
				{
					return true;
				}

				return false;
			}

			static void afterGettingFrame(void* clientData, unsigned frameSize, unsigned numTruncatedBytes, timeval presentationTime, unsigned durationInMicroseconds)
			{
				const auto sink = static_cast<DummySink*>(clientData);
				sink->afterGettingFrame(frameSize, numTruncatedBytes, presentationTime, durationInMicroseconds);
			}

			void afterGettingFrame(unsigned frameSize, unsigned numTruncatedBytes, timeval presentationTime, unsigned durationInMicroseconds)
			{
				if (fSubsession.rtpSource()->curPacketMarkerBit())
				{
					int nPosStart = 0;
					if( FindStartCode3(fReceiveBuffer.data()) )
					{
						nPosStart = 3;
					}
					else if ( FindStartCode4(fReceiveBuffer.data()) )
					{
						nPosStart = 4;
					}


					// Insert SPS , PPS
					uint8_t extraData[256] = "";
					uint16_t extraLen = 0;
					constexpr uint8_t nalu_header[4] = { 0, 0, 0, 1 };

					if (parent.GetMediaType() == DevLib::MultiMedia::MediaType::H264)
					{
						const int nNALType = fReceiveBuffer[nPosStart] & 0x1F;

						if (nNALType == NALTYPE_IDRPicture)
						{
							uint32_t num = 0;

							const auto param = fSubsession.fmtp_spropparametersets();
							const auto pSPropRecord = parseSPropParameterSets(param, num);

							if (param[0] != 0)
							{
								// p_record[0] is sps
								// p+record[1] is pps
								for (unsigned int i = 0; i < num; i++)
								{
									memcpy(&extraData[extraLen], &nalu_header[0], 4);
									extraLen += 4;
									memcpy(&extraData[extraLen], pSPropRecord[i].sPropBytes, pSPropRecord[i].sPropLength);
									extraLen += pSPropRecord[i].sPropLength;
								}
							}

							delete[] pSPropRecord;
						}

					}else if(parent.GetMediaType() == DevLib::MultiMedia::MediaType::H265)
					{
						const int nal_unit_type = (fReceiveBuffer[nPosStart] >> 1) & 0x3F; // 6-bit nal_unit_type

						if (bool isForbiddenBitSet = (fReceiveBuffer[nPosStart] & 0x80) != 0) // check  forbidden bit
						{
							continuePlaying();
							return;
						}

						if (nal_unit_type == 16 || nal_unit_type == 17 || nal_unit_type == 18 || nal_unit_type == 19 || nal_unit_type == 20 || nal_unit_type == 21 ) // IDR_W_RADL
						{
							const char* vps = fSubsession.fmtp_spropvps(); // VPS 
							const char* sps = fSubsession.fmtp_spropsps(); // SPS 
							const char* pps = fSubsession.fmtp_sproppps(); // PPS 

							if (vps) 
							{
								uint32_t numVPS;
								const auto pSPropRecord = parseSPropParameterSets(vps, numVPS); // VPS parse
								memcpy(&extraData[extraLen], &nalu_header[0], 4);
								extraLen += 4;
								memcpy(&extraData[extraLen], pSPropRecord[0].sPropBytes, pSPropRecord[0].sPropLength);
								extraLen += pSPropRecord[0].sPropLength;

								delete[] pSPropRecord;
							}

							if (sps) 
							{
								uint32_t numSPS;
								const auto pSPropRecord = parseSPropParameterSets(sps, numSPS); // SPS parse
								memcpy(&extraData[extraLen], &nalu_header[0], 4);
								extraLen += 4;
								memcpy(&extraData[extraLen], pSPropRecord[0].sPropBytes, pSPropRecord[0].sPropLength);
								extraLen += pSPropRecord[0].sPropLength;

								delete[] pSPropRecord;
							}

							if (pps) 
							{
								uint32_t numPPS;
								const auto pSPropRecord = parseSPropParameterSets(pps, numPPS); // PPS parse
								memcpy(&extraData[extraLen], &nalu_header[0], 4);
								extraLen += 4;
								memcpy(&extraData[extraLen], pSPropRecord[0].sPropBytes, pSPropRecord[0].sPropLength);
								extraLen += pSPropRecord[0].sPropLength;

								delete[] pSPropRecord;
							}
						}
					}
					

					// Insert NAL Start Header
					if (nPosStart == 0 )
					{
						memcpy(&extraData[extraLen], &nalu_header[0], 4);
						extraLen += 4;
					}

					// Shared PTR 
					std::shared_ptr<CRTSPClient::NalsPack> nalPtr = std::make_shared<CRTSPClient::NalsPack>();

					// Check Padding
					uint32_t size = extraLen + frameSize;
					if (size % 4 != 0) // 
					{
						size += (4 - size % 4);
					}

					nalPtr->second.resize(size);
					if( extraLen ) memcpy(nalPtr->second.data(), extraData, extraLen);
					memcpy(nalPtr->second.data() + extraLen, fReceiveBuffer.data(), frameSize + 0);

					// Calc FPS
					nalPtr->first = presentationTime.tv_sec + presentationTime.tv_usec * 0.000001;

					if (nFrameCount > 30)
					{
						avg_elapsed = avg_elapsed * 0.97 + (nalPtr->first - fps_prev_elapsed) * 0.03;
						parent.m_fps = 1.0 / avg_elapsed;
					}

					fps_prev_elapsed = nalPtr->first;


					parent.m_nals.Lock();

					if (parent.m_maxBuffSize > parent.m_nals->size())
					{
						parent.m_nals->push(std::move(nalPtr));
					}
					parent.m_nals.UnLock();

					(void)parent.m_eReceived.SetEvent();

					// Monitoring received
					parent.m_totalReceived.LockRead();
					*parent.m_totalReceived += frameSize;
					parent.m_totalReceived.UnLockRead();

					nFrameCount++;
				}
				//else
				//{
				//	if (parent.GetMediaType() == DevLib::MultiMedia::MediaType::H264)
				//	{
				//		const int nNALType = fReceiveBuffer[0] & 0x1F;
				//		printf("NalType : %d (%d)\n", nNALType, frameSize);
				//	}
				//	else if (parent.GetMediaType() == DevLib::MultiMedia::MediaType::H265)
				//	{
				//		const int nNALType = fReceiveBuffer[0] & 0x3F;
				//		printf("NalType : %d (%d)\n", nNALType, frameSize);
				//	}
				//}

				// Then continue, to request the next frame of data:
				continuePlaying();
			}

			// redefined virtual functions:
			Boolean continuePlaying() override
			{
				if (fSource == nullptr)
				{
					return False; // sanity check (should not happen)
				}

				// Request the next frame of data from our input source.  "afterGettingFrame()" will get called later, when it arrives:
				fSource->getNextFrame(fReceiveBuffer.data(), DUMMY_SINK_RECEIVE_BUFFER_SIZE, afterGettingFrame, this, onSourceClosure, this);

				return True;
			}

			int64_t nFrameCount;
			int64_t nFrameCountPrev;
			std::vector<uint8_t> fReceiveBuffer;
			MediaSubsession& fSubsession;
			CElapseTimer m_eTime;
			char* fStreamId;
			CRTSPClient& parent;

			double fps_prev_elapsed{ DBL_MIN };
			double avg_elapsed{ 0.033 };
		};

		// RTSP 'response handlers':
		void continueAfterDESCRIBE(RTSPClient* rtspClient, int resultCode, char* resultString);
		void continueAfterSETUP(RTSPClient* rtspClient, int resultCode, char* resultString);
		void continueAfterPLAY(RTSPClient* rtspClient, int resultCode, char* resultString);

		// Other event handler functions:
		void subsessionAfterPlaying(void* clientData); // called when a stream's subsession (e.g., audio or video substream) ends
		void subsessionByeHandler(void* clientData, char const* reason);
		// called when a RTCP "BYE" is received for a subsession
		void streamTimerHandler(void* clientData);
		// called at the end of a stream's expected duration (if the stream has not already signaled its end using a RTCP "BYE")

	  // The main streaming routine (for each "rtsp://" URL):
		void openURL(UsageEnvironment& env, char const* progName, char const* rtspURL);

		// Used to iterate through each stream's 'subsessions', setting up each one:
		void setupNextSubsession(RTSPClient* rtspClient);

		// Used to shut down and close a stream (including its "RTSPClient" object):
		void shutdownStream(RTSPClient* rtspClient, int exitCode = 1);

		inline CRTSPClientWrapper* toRTSPClient(handle_t rtsp) { return static_cast<CRTSPClientWrapper*>(rtsp); }
		inline TaskScheduler* toTaskScheduler(handle_t handle) { return static_cast<TaskScheduler*>(handle); }
		inline BasicUsageEnvironment* toBasicUsageEnvironment(handle_t handle) { return static_cast<BasicUsageEnvironment*>(handle); }

		CRTSPClient::CRTSPClient(uint32_t bufferSize)
			: m_msRTPTimeout(3000), m_totalReceived({}), bFragClosed(0), mediaType()
		{
			m_eReceived.Create();
		}

		CRTSPClient::~CRTSPClient()
		{
			Destroy();
			m_eReceived.Destroy();
		}

		void CRTSPClient::Destroy()
		{
			if (IsCreated())
			{
				//if (toRTSPClient(m_rtspClient)->bConnected)
				{
					bFragClosed = 1;
					
					m_watchDogEvent.WaitForEndThread(3000);
					
					m_processNals.StopThread();
					m_processNals.WaitForEndThread(3000);

					if (m_rtspClient)shutdownStream(toRTSPClient(m_rtspClient));
					
					m_nals.Lock();
					std::queue<std::shared_ptr<NalsPack>> empt;
					std::swap(empt, *m_nals);
					m_nals.UnLock();

					m_fps = 0;
				}
			}
		}

        void CRTSPClient::EnableDebug(bool bEnable)
        {
			m_bEnableDebug = bEnable;

			if( IsCreated() ) toRTSPClient(m_rtspClient)->SetDebug(m_bEnableDebug);
        }

        void CRTSPClient::SetTimeout(uint32_t msTime)
        {
			m_msRTPTimeout = msTime;

			if (IsCreated())
			{
				toRTSPClient(m_rtspClient)->rtpTimeout = m_msRTPTimeout;
			}
		}

        bool CRTSPClient::StartVideoLog(const std::string& filename, int32_t width, int32_t height, int32_t fps)
        {
			return m_videoWriter.CreateVideoFile(filename, width, height, fps, mediaType);
        }

		std::string CRTSPClient::GetVideoFileName()
		{
			return m_videoWriter.GetVideoFileName();
		}

		uint32_t CRTSPClient::GetVideoFrameCount() const
		{
			return m_videoWriter.GetVideoFrameCount();
		}

		void CRTSPClient::StopVideoLog()
        {
			m_videoWriter.DestroyVideoFile();
        }

        double CRTSPClient::GetFPS() const
        {
			return m_fps;
        }

        void CRTSPClient::EnableMonitoring()
        {
			m_timerMonitoring.StartTimer(1000, &CRTSPClient::OnTimerMonitoring, this);
        }

        void CRTSPClient::DisableMonitoring()
        {
			m_timerMonitoring.StopTimer();
        }

        void CRTSPClient::watchDogEvent()
        {
			toTaskScheduler(m_scheduler)->doEventLoop(&bFragClosed);

			if (toRTSPClient(m_rtspClient)->session)
			{
				Medium::close(toRTSPClient(m_rtspClient)->session);
				Medium::close(toRTSPClient(m_rtspClient));
			}

			toBasicUsageEnvironment(m_environment)->reclaim();

			// delete m_rtspClient;
			delete toTaskScheduler(m_scheduler);
			// delete m_environment;

			m_environment = nullptr;
			m_scheduler = nullptr;
			m_rtspClient = nullptr;
		}

        void CRTSPClient::processNals()
        {
			while(m_processNals.IsRunThread())
			{
				if( m_eReceived.WaitForEvent(100) )
				{
					m_nals.Lock();
					auto queue = std::move(*m_nals);
					m_nals.UnLock();

					while (!queue.empty())
					{
						const auto qData = std::move(queue.front());
						queue.pop();

						if (m_callbackRTPData) m_callbackRTPData(qData->second, qData->first);

						m_videoWriter.WriteVideo(qData->second.data(), qData->second.size());
					}
				}
			}
        }

        void CRTSPClient::OnTimerMonitoring()
        {
			m_totalReceived.LockRead();
			const auto nReceived = *m_totalReceived;
			*m_totalReceived = 0;
			m_totalReceived.UnLockRead();

			if (m_callbackMonitoring) m_callbackMonitoring(GetFPS(), static_cast<double>(nReceived) * 8.0 / 1024);
        }

		void CRTSPClient::OnClose(const char_t* sessionName)
		{
			bFragClosed = 1;

			if (m_callbackDisConnected) m_callbackDisConnected(m_rtspURL, std::string(sessionName));
		}

		void CRTSPClient::OnConnection(const char_t* sessionName) const
		{
			if (m_callbackConnected) m_callbackConnected(m_rtspURL, std::string(sessionName));
		}

		bool CRTSPClient::CreateRTSPClient(const std::string& rtspURL, uint32_t msRTPTimeout, DevLib::MultiMedia::MediaType type, bool bUseOverTCP)
		{
			bool bRet = false;

			if (IsCreated() == false)
			{
				m_scheduler = BasicTaskScheduler::createNew();
				m_environment = BasicUsageEnvironment::createNew(*toTaskScheduler(m_scheduler));

				if (m_environment)
				{
					uint32_t bDebug = 0;
					if (m_bEnableDebug)
					{
						bDebug = 1;
					}

					m_rtspClient = CRTSPClientWrapper::createNew(*this, *toBasicUsageEnvironment(m_environment), rtspURL.c_str(), bDebug, GetProcessName().c_str());

					if (m_rtspClient)
					{

						m_msRTPTimeout = msRTPTimeout;
						toRTSPClient(m_rtspClient)->rtpTimeout = m_msRTPTimeout;
						toRTSPClient(m_rtspClient)->SetUseStreamOverTCP(bUseOverTCP);
						m_rtspURL = rtspURL;
						toRTSPClient(m_rtspClient)->RegisterCallbackClosed(&CRTSPClient::OnClose, this);
						toRTSPClient(m_rtspClient)->RegisterCallbackConnection(&CRTSPClient::OnConnection, this);

						mediaType = type;

						// Proceess Thread
						m_processNals.StartThread(&CRTSPClient::processNals, this);

						// Event Loop
						bFragClosed = 0;

						m_watchDogEvent.StartThread(&CRTSPClient::watchDogEvent, this);

						bRet = true;
					}
				}
			}

			return bRet;
		}

		bool CRTSPClient::IsCreated() const
		{
			bool bRet = false;
			if (m_rtspClient) bRet = true;

			return bRet;
		}

		bool CRTSPClient::TryConnect()
		{
			bool bRet = false;

			if (IsCreated())
			{
				if (!IsConnected())
				{
					if (toRTSPClient(m_rtspClient)->bTryConnection == false)
					{
						toRTSPClient(m_rtspClient)->sendDescribeCommand(continueAfterDESCRIBE);
						toRTSPClient(m_rtspClient)->bTryConnection = true;

						bRet = true;
					}
				}
			}
			else
			{
				if (!m_rtspURL.empty())
				{
					if (CreateRTSPClient(m_rtspURL, m_msRTPTimeout))
					{
						if (toRTSPClient(m_rtspClient)->bTryConnection == false)
						{
							toRTSPClient(m_rtspClient)->sendDescribeCommand(continueAfterDESCRIBE);
							toRTSPClient(m_rtspClient)->bTryConnection = true;

							bRet = true;
						}
					}
				}
			}

			return bRet;
		}

		bool CRTSPClient::IsConnected() const
		{
			bool bRet = false;
			if (IsCreated())
			{
				if (toRTSPClient(m_rtspClient)->bConnected)
				{
					bRet = true;
				}
			}

			return bRet;
		}

		void continueAfterDESCRIBE(RTSPClient* rtspClient, int resultCode, char* resultString)
		{
			UsageEnvironment& env = rtspClient->envir(); // alias

			if (resultCode == 0)
			{
				toRTSPClient(rtspClient)->bConnected = true;

				const char* const sdpDescription = resultString;
				//env << "Got a SDP description:\n"<< sdpDescription << "\n";

				// Create a media session object from this SDP description:
				if (toRTSPClient(rtspClient)->session == nullptr) toRTSPClient(rtspClient)->session = MediaSession::createNew(env, sdpDescription);
				//delete[] sdpDescription; // because we don't need it anymore

				toRTSPClient(rtspClient)->Connection(); 

				if (toRTSPClient(rtspClient)->session)
				{
					if (toRTSPClient(rtspClient)->session->hasSubsessions())
					{
						toRTSPClient(rtspClient)->iter = new MediaSubsessionIterator(*toRTSPClient(rtspClient)->session);
						setupNextSubsession(rtspClient);
					}
					else
					{
						env << "This session has no media subsessions (i.e., no \"m=\" lines";
					}
				}
				else
				{
					env << "Failed to create a MediaSession object from the SDP description: \n" << env.getResultMsg() << "\n";
				}

			}
			else
			{
				shutdownStream(toRTSPClient(rtspClient));
			}

			delete[] resultString;

			toRTSPClient(rtspClient)->bTryConnection = false;
		}

		void setupNextSubsession(RTSPClient* rtspClient)
		{
			toRTSPClient(rtspClient)->subsession = toRTSPClient(rtspClient)->iter->next();

			if (toRTSPClient(rtspClient)->subsession != nullptr)
			{
				if (!toRTSPClient(rtspClient)->subsession->initiate())
				{
					setupNextSubsession(rtspClient); // give up on this subsession; go to the next one
				}
				else
				{
					// Continue setting up this subsession, by sending a RTSP "SETUP" command:
					rtspClient->sendSetupCommand(*toRTSPClient(rtspClient)->subsession, continueAfterSETUP, False, toRTSPClient(rtspClient)->UsedStreamOverTCP());
				}
				return;
			}

			// We've finished setting up all of the subsessions.  Now, send a RTSP "PLAY" command to start the streaming:
			if (toRTSPClient(rtspClient)->session->absStartTime() != nullptr)
			{
				// Special case: The stream is indexed by 'absolute' time, so send an appropriate "PLAY" command:
				rtspClient->sendPlayCommand(*toRTSPClient(rtspClient)->session, continueAfterPLAY, toRTSPClient(rtspClient)->session->absStartTime(), toRTSPClient(rtspClient)->session->absEndTime());
			}
			else
			{
				toRTSPClient(rtspClient)->duration = toRTSPClient(rtspClient)->session->playEndTime() - toRTSPClient(rtspClient)->session->playStartTime();
				rtspClient->sendPlayCommand(*toRTSPClient(rtspClient)->session, continueAfterPLAY);
			}
		}

		void continueAfterSETUP(RTSPClient* rtspClient, int resultCode, char* resultString)
		{
			if (resultCode == 0)
			{
				UsageEnvironment& env = rtspClient->envir(); // alias

				// Having successfully setup the subsession, create a data sink for it, and call "startPlaying()" on it.
				// (This will prepare the data sink to receive data; the actual flow of data from the client won't start happening until later,
				// after we've sent a RTSP "PLAY" command.)

				toRTSPClient(rtspClient)->subsession->sink = DummySink::createNew(toRTSPClient(rtspClient)->parent, env, *toRTSPClient(rtspClient)->subsession, rtspClient->url());

				// perhaps use your own custom "MediaSink" subclass instead
				if (toRTSPClient(rtspClient)->subsession->sink == nullptr)
				{
					setupNextSubsession(rtspClient);
				}

				toRTSPClient(rtspClient)->subsession->miscPtr = rtspClient; // a hack to let subsession handler functions get the "RTSPClient" from the subsession 
				toRTSPClient(rtspClient)->subsession->sink->startPlaying(*(toRTSPClient(rtspClient)->subsession->readSource()), subsessionAfterPlaying, toRTSPClient(rtspClient)->subsession);

				// Also set a handler to be called if a RTCP "BYE" arrives for this subsession:
				if (toRTSPClient(rtspClient)->subsession->rtcpInstance() != nullptr)
				{
					toRTSPClient(rtspClient)->subsession->rtcpInstance()->setByeWithReasonHandler(subsessionByeHandler, toRTSPClient(rtspClient)->subsession);
				}
			}

			delete[] resultString;

			// Set up the next subsession, if any:
			setupNextSubsession(rtspClient);
		}

		void continueAfterPLAY(RTSPClient* rtspClient, int resultCode, char* resultString)
		{
			if (resultCode == 0)
			{
				const UsageEnvironment& env = rtspClient->envir(); // alias

				// Set a timer to be handled at the end of the stream's expected duration (if the stream does not already signal its end
				// using a RTCP "BYE").  This is optional.  If, instead, you want to keep the stream active - e.g., so you can later
				// 'seek' back within it and do another RTSP "PLAY" - then you can omit this code.
				// (Alternatively, if you don't want to receive the entire stream, you could set this timer for some shorter value.)

				if (toRTSPClient(rtspClient)->rtpTimeout > 0)
				{
					const auto uSecsToDelay = static_cast<unsigned>(toRTSPClient(rtspClient)->rtpTimeout * 1000);
					toRTSPClient(rtspClient)->streamTimerTask = env.taskScheduler().scheduleDelayedTask(uSecsToDelay, (TaskFunc*)streamTimerHandler, rtspClient);
				}
			}
			else
			{
				shutdownStream(toRTSPClient(rtspClient));
			}

			delete[] resultString;

		}

		// Implementation of the other event handlers:
		void subsessionAfterPlaying(void* clientData)
		{
			auto subsession = static_cast<MediaSubsession*>(clientData);
			// const auto rtspClient = static_cast<RTSPClient*>(subsession->miscPtr);

			// Begin by closing this subsession's stream:
			Medium::close(subsession->sink);
			subsession->sink = nullptr;

			// Next, check whether *all* subsessions' streams have now been closed:
			const MediaSession& session = subsession->parentSession();
			MediaSubsessionIterator iter(session);
			while ((subsession = iter.next()) != nullptr)
			{
				if (subsession->sink != nullptr) return; // this subsession is still active
			}
		}

		void subsessionByeHandler(void* clientData, char const* reason)
		{
			const auto subsession = static_cast<MediaSubsession*>(clientData);
			const RTSPClient* rtspClient = static_cast<RTSPClient*>(subsession->miscPtr);
			UsageEnvironment& env = rtspClient->envir(); // alias

			if (reason != nullptr)
			{
				env << " (reason:\"" << reason << "\")";
				delete[]const_cast<char*>(reason);
			}

			// Now act as if the subsession had closed:
			subsessionAfterPlaying(subsession);
		}

		void streamTimerHandler(void* clientData)
		{
			if (toRTSPClient(clientData)->session != nullptr)
			{
				MediaSubsessionIterator iter(*toRTSPClient(clientData)->session);
				MediaSubsession* subsession;

				bool bCheckUpdate = true;
				bool bShutdown = true;

				// check update frame
				while ((subsession = iter.next()) != nullptr)
				{
					if (subsession->sink != nullptr)
					{
						const auto sink = dynamic_cast<DummySink*>(subsession->sink);

						if (sink->IsUpdateFrame() == true)
						{
							bShutdown = false;
						}
					}
				}

				// connection close
				if (bShutdown)
				{
					bCheckUpdate = false;
					shutdownStream(toRTSPClient(clientData));
					toRTSPClient(clientData)->streamTimerTask = nullptr;
				}

				// next update check : timer
				if (bCheckUpdate)
				{
					toRTSPClient(clientData)->streamTimerTask = toRTSPClient(clientData)->envir().taskScheduler().scheduleDelayedTask(static_cast<int64_t>(toRTSPClient(clientData)->rtpTimeout * 1000), streamTimerHandler, toRTSPClient(clientData));
				}
			}
		}

		void shutdownStream(RTSPClient* rtspClient, int exitCode)
		{
			toRTSPClient(rtspClient)->bShutdown = true;
			// UsageEnvironment& env = rtspClient->envir(); // alias
			if (rtspClient)
			{
				// First, check whether any subsessions have still to be closed:
				if (toRTSPClient(rtspClient)->session != nullptr)
				{
					Boolean someSubSessionsWereActive = False;
					MediaSubsessionIterator itr(*toRTSPClient(rtspClient)->session);
					MediaSubsession* subSession;

					while ((subSession = itr.next()) != nullptr)
					{
						if (subSession->sink != nullptr)
						{
							Medium::close(subSession->sink);
							subSession->sink = nullptr;

							if (subSession->rtcpInstance() != nullptr)
							{
								subSession->rtcpInstance()->setByeHandler(nullptr, nullptr); // in case the server sends a RTCP "BYE" while handling "TEARDOWN"
							}

							someSubSessionsWereActive = True;
						}
					}

					if (someSubSessionsWereActive)
					{
						// Send a RTSP "TEARDOWN" command, to tell the server to shutdown the stream.
						// Don't bother handling the response to the "TEARDOWN".
						rtspClient->sendTeardownCommand(*toRTSPClient(rtspClient)->session, nullptr);
					}

					toRTSPClient(rtspClient)->bConnected = false;
					toRTSPClient(rtspClient)->Close();
				}
			}
		}

	}
}