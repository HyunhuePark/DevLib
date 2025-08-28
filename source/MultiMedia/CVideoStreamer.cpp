#include <utility>

#include "../../include/MultiMedia/CVideoStreamer.hpp"


namespace DevLib
{
	namespace MultiMedia
	{
		CVideoStreamer::CVideoStreamer(std::string sessionName, bool EnableAuthentication)
			: m_server(std::move(sessionName), EnableAuthentication)
		{
		}

		CVideoStreamer::~CVideoStreamer()
		{
			Destroy();
		}

		bool CVideoStreamer::CreateStreamer(uint16_t serverPort, int32_t width, int32_t height, int64_t bitRateKB, int32_t frameRate, int32_t gopSize, CodecType type, Graphics::ImageType inputType, CodecPreset preset, CodecProfile profile, const string_t& optURL )
		{
			return CreateStreamerEx(serverPort, width, height, bitRateKB, frameRate, gopSize, CodecTypeToString(type), inputType, preset, profile, optURL);
		}

		bool CVideoStreamer::CreateStreamerEx(uint16_t serverPort, int32_t width, int32_t height, int64_t bitRateKB, int32_t frameRate, int32_t gopSize, const std::string& strType, Graphics::ImageType inputType, CodecPreset preset, CodecProfile profile, const string_t& optURL)
		{
			bool bRet = false;

			if (m_encoder.CreateEncoderEx(width, height, bitRateKB, frameRate, gopSize, strType, inputType, preset, profile))
			{
				if (m_server.CreateRTSPServer(serverPort, CodecStringToMediaType(strType), optURL))
				{
					bRet = true;
				}
				else
				{
					m_encoder.Destroy();
				}
			}

			return bRet;
		}

		bool CVideoStreamer::SwitchVideoSource(int32_t width, int32_t height, int64_t bitRateKB, int32_t frameRate, int32_t gopSize, CodecType type, Graphics::ImageType inputType, CodecPreset preset , CodecProfile profile )
		{
			return SwitchVideoSourceEx(width, height, bitRateKB, frameRate, gopSize, CodecTypeToString(type), inputType, preset, profile);
		}

		bool CVideoStreamer::SwitchVideoSourceEx(int32_t width, int32_t height, int64_t bitRateKB, int32_t frameRate, int32_t gopSize, const std::string& strType, Graphics::ImageType inputType, CodecPreset preset, CodecProfile profile)
		{
			m_encoder.Destroy();
			return m_encoder.CreateEncoderEx(width, height, bitRateKB, frameRate, gopSize, strType, inputType, preset, profile);
		}

		void CVideoStreamer::Destroy()
		{
			m_encoder.Destroy();
			m_server.Destroy();
		}

		void CVideoStreamer::StreamImage(const Graphics::CImageObject& src, const uint32_t sec, const uint32_t usec)
		{
			if( m_encoder.Encode(src.GetImageConst()) )
			{
				m_server.StreamNal(m_encoder.GetEncodedData(), m_encoder.GetEncodedSize(), sec, usec);
			}
		}

		bool CVideoStreamer::IsCreated() const
		{
			return m_encoder.IsCreated() && m_server.IsCreated();
		}

		bool CVideoStreamer::StartVideoLog(const std::string& filename)
		{
			return m_encoder.StartVideoLog(filename);
		}

		std::string CVideoStreamer::GetVideoFileName()
		{
			return m_encoder.GetVideoFileName();
		}

		uint32_t CVideoStreamer::GetVideoFrameCount() const
		{
			return m_encoder.GetVideoFrameCount();
		}

		void CVideoStreamer::StopVideoLog()
		{
			m_encoder.StopVideoLog();
		}

        bool CVideoStreamer::UseGPU(uint32_t idxGPU)
        {
            return m_encoder.UseGPU(idxGPU);
        }

        void CVideoStreamer::UpdateBitRate(int32_t bitRate) const
        {
			m_encoder.UpdateBitRate(bitRate);
        }

        void CVideoStreamer::UpdateFrameRate(int32_t frameRate, int32_t gopSize) const
        {
			m_encoder.UpdateFrameRate(frameRate, gopSize);
        }

        bool CVideoStreamer::AddUser(const std::string &userName, const std::string &password) const
        {
			return m_server.AddUser(userName, password);
		}

		bool CVideoStreamer::RemoveUser(const std::string& userName) const
		{
			return m_server.RemoveUser(userName);
		}
	}
}