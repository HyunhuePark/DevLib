#pragma once

#include <map>
#include <string>

#include "../../Base/CCriticalSection.hpp"
#include "../../IO/CTcpClient.hpp"
#include "DevLibParameterType.hpp"

namespace DevLib {
	namespace Routine
	{
		class CParameterClient
		{
		public:
			CParameterClient();
			virtual ~CParameterClient() = default;

			bool CreateClient(const std::string& serverIP, uint16_t serverPort);
			bool RequestParameter();

			// GroupName, ParameterName, ParameterType, ParameterValue
			using ParameterInfo = std::tuple<std::string, std::string, std::string, std::string>;

			std::vector<ParameterInfo> GetParameterInfo() const;

			template <typename Type>
			bool SetValue(std::string GroupName, std::string Name, Type Value);

			EnableCallback(Connection, const std::string& ip, uint16_t port)
			EnableCallback(DisConnection, const std::string& ip, uint16_t port)

		private:
			IO::CTcpClient m_client;
			CCriticalSection m_lock;
			std::map<std::string, std::map<std::string, std::pair<std::string, std::string>>> m_params;

			void OnConnectionServer(const std::string& ip, uint16_t port);
			void OnDisConnectionServer(const std::string& ip, uint16_t port) const;

			void OnReceiveData(const IO::CSocket& sock, const std::string& ip, uint16_t port, void_ptr pData, int32_t size);

			void UpdateParameter(const std::string& GroupName, const std::string& Name, const std::string& value);
		};

		template <typename Type>
		bool CParameterClient::SetValue(std::string GroupName, std::string Name, Type Value)
		{
			bool bRet = false;
			if (m_params.find(GroupName) != m_params.end())
			{
				if (m_params[GroupName].find(Name) != m_params[GroupName].end())
				{
					const auto& ParamType = m_params[GroupName][Name].first;
					auto& ParamValue = m_params[GroupName][Name].second;

					if (std::is_same_v<Type, bool_t> && ParamType == "bool_t")
					{
						bRet = true;
						if (Value) ParamValue = "true";
						else ParamValue = "false";

						UpdateParameter(GroupName, Name, ParamValue);
					}
					else if (std::is_same_v<Type, uint8_t> || std::is_same_v<Type, uint16_t> ||
						std::is_same_v<Type, uint32_t> || std::is_same_v<Type, uint64_t> ||
						std::is_same_v<Type, int8_t> || std::is_same_v<Type, int16_t> ||
						std::is_same_v<Type, int32_t> || std::is_same_v<Type, int64_t> ||
						std::is_same_v<Type, size_t> || std::is_same_v<Type, float_t> ||
						std::is_same_v<Type, double_t>)
					{
						bRet = true;
						ParamValue = std::to_string(Value);

						UpdateParameter(GroupName, Name, ParamValue);
					}

				}
			}
			return bRet;
		}
	}
}