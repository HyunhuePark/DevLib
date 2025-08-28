#include "../../include/MultiMedia/CVideoReceiver.hpp"
#include "../../include/Base/CElapseTimer.hpp"
namespace DevLib
{
	namespace MultiMedia
	{

		CVideoReceiver::CVideoReceiver(int nGPU)
			:m_timestampImage(0), m_decoder(nGPU)
		{
			m_client.RegisterCallbackRTPData(&CVideoReceiver::OnRTPNals, this);
			m_client.RegisterCallbackConnected(&CVideoReceiver::OnConnectedRTSP, this);
			m_client.RegisterCallbackDisConnected(&CVideoReceiver::OnDisConnectedRTSP, this);
			m_client.RegisterCallbackMonitoring(&CVideoReceiver::OnClientMonitoring, this);

			m_waitReceivedImage.Create();
		}

		CVideoReceiver::~CVideoReceiver()
		{
			Destroy();

			m_waitReceivedImage.Destroy();
		}

		void CVideoReceiver::Destroy()
		{
			m_decoder.Destroy();
			m_client.Destroy();
		}

		void CVideoReceiver::EnableDebug(bool bEnable)
		{
			m_client.EnableDebug(bEnable);
		}

		void CVideoReceiver::SetTimeout(uint32_t msTime)
		{
			m_client.SetTimeout(msTime);
		}

		bool CVideoReceiver::StartVideoLog(const std::string& filename, int32_t width, int32_t height, int32_t fps)
		{
			int32_t _fps = fps;

			if(_fps == 0 )
			{
				_fps = static_cast<int32_t>(m_client.GetFPS());
			}

			return m_decoder.StartVideoLog(filename, width, height, _fps);
		}

		std::string CVideoReceiver::GetVideoFileName()
		{
			return m_decoder.GetVideoFileName();
		}

		uint32_t CVideoReceiver::GetVideoFrameCount() const
		{
			return m_decoder.GetVideoFrameCount();
		}

		void CVideoReceiver::StopVideoLog()
		{
			m_decoder.StopVideoLog();
		}

		double CVideoReceiver::GetFPS() const
		{
			return m_client.GetFPS();
		}

		void CVideoReceiver::EnableMonitoring()
		{
			m_client.EnableMonitoring();
		}

		void CVideoReceiver::DisableMonitoring()
		{
			m_client.DisableMonitoring();
		}

		bool CVideoReceiver::UseGPU(uint32_t idxGPU)
        {
            return m_decoder.UseGPU(idxGPU);
        }

		void CVideoReceiver::OnClientMonitoring(const double& fps, const double& bitRate) const
		{
			if (m_callbackMonitoring) m_callbackMonitoring(fps, bitRate);
		}

        void CVideoReceiver::OnConnectedRTSP(const std::string &url, const std::string &sessionName) const
        {
			if (m_callbackConnected) m_callbackConnected(url, sessionName);
		}

		void CVideoReceiver::OnDisConnectedRTSP(const std::string& url, const std::string& sessionName) const
		{
			if (m_callbackDisConnected) m_callbackDisConnected(url, sessionName);
		}

		void CVideoReceiver::OnRTPNals(const std::vector<uint8_t>& nal, const double& timestamp)
		{
			if( m_decoder.Decode(nal.data(), nal.size()) > 0)
			{
				m_timestampImage = timestamp;

				(void)m_waitReceivedImage.SetEvent();

				if (m_callbackReceiverImage) m_callbackReceiverImage(m_decoder.GetDecodedImage(), m_timestampImage);
			}
		}

		bool CVideoReceiver::CreateReceiver(const std::string& rtspURL, uint32_t msRTPTimeout, bool bUseOverTCP, CodecType type, DevLib::Graphics::ImageType outputType, DevLib::MultiMedia::ConvertType cvtType)
		{
			bool bRet = false;

			if (m_decoder.CreateDecoder(type, outputType, cvtType))
			{
				if (m_client.CreateRTSPClient(rtspURL, msRTPTimeout, CodecTypeToMediaType(type), bUseOverTCP))
				{
					bRet = true;
				}
				else
				{
					m_decoder.Destroy();
				}
			}

			return bRet;
		}

		bool CVideoReceiver::IsCreated() const
		{
			bool bRet = false;
			if (m_decoder.IsCreated() && m_client.IsCreated()) bRet = true;

			return bRet;
		}

		bool CVideoReceiver::WaitForReceived(uint32_t waitTimeMS) const
		{
			return m_waitReceivedImage.WaitForEvent(waitTimeMS);
		}

		double_t CVideoReceiver::GetReceivedImage(DevLib::Graphics::CImage& image)
		{
			image = m_decoder.GetDecodedImage();
			return m_timestampImage;
		}

		bool CVideoReceiver::TryConnect()
		{
			return m_client.TryConnect();
		}

		bool CVideoReceiver::IsConnected() const
		{
			return m_client.IsConnected();
		}
	}
}