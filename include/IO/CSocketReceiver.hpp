#pragma once

#include "CSocket.hpp"
#include "../Base/DevLibCallback.hpp"

namespace DevLib
{
	namespace IO
	{
		class CSocketReceiver
		{
		public:
			CSocketReceiver();
			virtual ~CSocketReceiver() = default;

			bool Create(int32_t sockType = SOCK_TYPE_TCP, size_t headSize = 8192);
			bool IsCreated() const;

			int32_t Send(void_const_ptr pSrc, int32_t size, int32_t nFlag = 0) const;
			int32_t SendTo(void_const_ptr pSrc, int32_t size, const std::string& ip, uint16_t port, int32_t nFlag = 0) const;

			void SetBufferSize(size_t headSize);
			size_t GetBufferSize() const;
			CSocket& GetSocket();

			void StartWatchDog();

			EnableCallback(ReceivedData, CSocket& sock, void_ptr pData, int32_t dataSize)
			EnableCallback(SocketState, CSocket& sock, uint32_t SocketState)

		private:
			CSocket	 m_socket;
			size_t m_sizeBuffer;
			std::vector<uint8_t> m_dataBuffer;

			void OnReceive(CSocket& sock);

		public:
			CSocketReceiver(const CSocketReceiver& other) = delete;
			CSocketReceiver(CSocketReceiver&&) = default;

			CSocketReceiver& operator=(const CSocketReceiver&) = delete;
			CSocketReceiver& operator=(CSocketReceiver&&) = default;

			CSocket* operator->()
			{
				return &m_socket;
			}

			CSocket& operator*()
			{
				return m_socket;
			}

		};

	}
}
