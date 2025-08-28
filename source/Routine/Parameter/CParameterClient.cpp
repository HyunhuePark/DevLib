#include "../../../include/Routine/Parameter/CParameterClient.hpp"
#include "../../../include/Routine/Parameter/DevLibParameterType.hpp"
#include "../../../include/Routine/Network/Message/RoutineMessageType.hpp"

namespace DevLib {
	namespace Routine
	{
		CParameterClient::CParameterClient()
		{
			m_client.RegisterCallbackConnection(&CParameterClient::OnConnectionServer, this);
			m_client.RegisterCallbackDisConnection(&CParameterClient::OnDisConnectionServer, this);

			m_client.RegisterCallbackReceiveData(&CParameterClient::OnReceiveData, this);

			m_client.SetBufferSize(sizeof(RoutineMessageHeader));
		}

		bool CParameterClient::CreateClient(const std::string& serverIP, uint16_t serverPort)
		{
			return m_client.CreateTcpClient(serverIP, serverPort);
		}

		bool CParameterClient::RequestParameter()
		{
			const std::string req = "list";
			const int32_t nSize = m_client.Send(req.data(), req.size());

			return nSize == static_cast<int32_t>(req.size()) ? true : false;
		}

		std::vector<CParameterClient::ParameterInfo> CParameterClient::GetParameterInfo() const
		{
			std::vector<ParameterInfo> vInfo;

			m_lock.Lock();
			for (const auto& group : m_params)
			{
				for (const auto& param : group.second)
				{
					ParameterInfo info(group.first.c_str(), param.first.c_str(), param.second.first.c_str(), param.second.second.c_str());
					vInfo.emplace_back(info);
				}
			}
			m_lock.UnLock();

			return vInfo;
		}

		void CParameterClient::OnConnectionServer(const std::string& ip, const uint16_t port)
		{
			if (m_callbackConnection) m_callbackConnection(ip, port);
			RequestParameter();
		}

		void CParameterClient::OnDisConnectionServer(const std::string& ip, const uint16_t port) const
		{
			if (m_callbackDisConnection) m_callbackDisConnection(ip, port);
		}

		void CParameterClient::OnReceiveData(const IO::CSocket& sock, const std::string& ip, uint16_t port, void_ptr pData, int32_t size)
		{
			const auto pHeader = static_cast<RoutineMessageHeader*>(pData);

			std::vector<uint8_t> buffer;
			buffer.resize(pHeader->MessageLength);

			size_t nBuffPos = 0;

			while (pHeader->MessageLength - nBuffPos)
			{
				const int32_t nRecvSize = sock.Recv(&buffer[nBuffPos], pHeader->MessageLength - nBuffPos);
				if (nRecvSize <= 0) return;
				nBuffPos += nRecvSize;
			}

			if (pHeader->MessageID == 0)
			{
				ParameterPacket packet;
				CDeSerializer de_serializer(buffer.data());
				de_serializer >> packet;

				m_lock.Lock();
				for (const auto& param : packet.parameters)
				{
					m_params[param.GroupName][param.Name] = std::make_pair(param.Type, param.Value);
				}
				m_lock.UnLock();
			}

		}

		void CParameterClient::UpdateParameter(const std::string& GroupName, const std::string& Name, const std::string& value)
		{
			const std::string update = "update " + GroupName + "." + Name + "=" + value;

			m_client.Send(update.data(), update.size());
		}
	}
}