#include "../../include/IO/CSocketReceiver.hpp"
#include "../../include/IO/DevLibSocketUtility.hpp"

namespace DevLib {
	namespace IO {

		CSocketReceiver::CSocketReceiver()
			: m_sizeBuffer(8192)
		{
			m_socket.RegisterCallbackReceive(&CSocketReceiver::OnReceive, this);
			RegisterCallbackReceivedData(&CSocketReceiver::OnReceivedData, this);
			RegisterCallbackSocketState(&CSocketReceiver::OnSocketState, this);
		}

		bool CSocketReceiver::Create(int32_t sockType /*= SOCK_TYPE_TCP*/, size_t headSize /*= 8192*/)
		{
			SetBufferSize(headSize);

			return m_socket.Create(sockType);
		}

		bool CSocketReceiver::IsCreated() const
		{
			return m_socket.IsCreated();
		}

		int32_t CSocketReceiver::Send(void_const_ptr pSrc, int32_t size, int32_t nFlag) const
		{
			return m_socket.Send(pSrc, size, nFlag);
		}

		int32_t CSocketReceiver::SendTo(void_const_ptr pSrc, int32_t size, const std::string& ip, uint16_t port, int32_t nFlag) const
		{
			return m_socket.SendTo(pSrc, size, ip, port, nFlag);
		}

		void CSocketReceiver::SetBufferSize(size_t headSize)
		{
			m_sizeBuffer = headSize;
			m_dataBuffer.resize(m_sizeBuffer);
		}

		size_t CSocketReceiver::GetBufferSize() const
		{
			return m_sizeBuffer;
		}

		CSocket& CSocketReceiver::GetSocket()
		{
			return m_socket;
		}

		void CSocketReceiver::StartWatchDog()
		{
			m_socket.StartWatchDog();
		}

		void CSocketReceiver::OnReceive(CSocket& sock)
		{
			int32_t nRecv;
			if (m_socket.GetSocketType() == SOCK_TYPE_TCP)
			{
				nRecv = sock.Recv(m_dataBuffer.data(), static_cast<int32_t>(m_dataBuffer.size()));
			}
			else // SOCK_TYPE_UDP
			{
				SOCK_ADDR addr{};
				nRecv = sock.RecvFrom(m_dataBuffer.data(), static_cast<int32_t>(m_dataBuffer.size()), addr);
			}

			if (nRecv > 0)
			{
				m_callbackReceivedData(sock, m_dataBuffer.data(), nRecv);
			}
			else
			{
#if defined( _MSC_VER )
				m_callbackSocketState(sock, WSAGetLastError());
#elif defined(__linux__)	//Linux
				m_callbackSocketState(sock, errno);
#endif
			}
		}

	}
}
