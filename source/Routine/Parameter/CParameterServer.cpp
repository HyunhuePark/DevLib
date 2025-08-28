#include "../../../include/Routine/Parameter//CParameterServer.hpp"

#include "../../../include/Routine/Network/Message/RoutineMessageType.hpp"
#include <cjson/cJSON.h>
#include "../../../include/Utility/Config.hpp"

#include "../../../include/IO/DevLibSocketUtility.hpp"
#include <sstream>

namespace DevLib {
	namespace Routine
	{
		CParameterServer::CParameterServer()
		{
			m_server.RegisterCallbackRequestGet(&CParameterServer::OnRequestGET, this);
			m_server.RegisterCallbackRequestEx(&CParameterServer::OnRequestUser, this);
			m_server.RegisterCallbackConnection(&CParameterServer::OnConnection, this);
			m_server.RegisterCallbackDisConnection(&CParameterServer::OnDisConnection, this);
		}

		bool CParameterServer::ServerOpen(uint16_t port)
		{
			return m_server.HTTPOpen(port);
		}

		void CParameterServer::SaveParameter() const
		{
			for (const auto& group : m_params)
			{
				for (const auto& paramSet : group.second)
				{
					const std::string type = paramSet.second->ToStringType();
					if (type == "bool_t")
					{
						SetConfigString(group.first, paramSet.first, paramSet.second->ToStringValue());
					}
					else if (type == "double_t" || type == "float_t")
					{
						char* pEnd;
						const auto value = std::strtod(paramSet.second->ToStringValue().c_str(), &pEnd);
						if(*pEnd == '\0') SetConfigDouble(group.first, paramSet.first, value);
					}
					else if( type == "string_t")
					{
						SetConfigString(group.first, paramSet.first, paramSet.second->ToStringValue());
					}
					else
					{
						char* pEnd;
						const auto value = std::strtol(paramSet.second->ToStringValue().c_str(), &pEnd, 10);
						if (*pEnd == '\0') SetConfigInt(group.first, paramSet.first, value);
					}
				}
			}
		}

		void CParameterServer::OnRequestGET(const IO::CSocket& client, HTTP::HeaderProperty& property, const std::string& url)
		{
			std::string page;
			std::string params;
			std::istringstream ss(url);
			std::getline(ss, page, '?');
			std::getline(ss, params, '?');

			if (page == "/" || page.empty())
			{
				std::string html = MakeMain();
				(void)client.Send(html.c_str(), html.size());
			}
			else if (page == "/json")
			{
				std::string json = MakeJSon();
				std::string http = MakeHTTP(200, json, "application/json");
				(void)client.Send(http.c_str(), http.size());
			}
			else if (page == "/save")
			{
				SaveParameter();
				std::string html = MakeRedirect("/");
				(void)client.Send(html.c_str(), html.size());
			}
			else if (page == "/change")
			{
				std::istringstream ssParam(params);
				std::string gParams;
				std::string value;

				std::getline(ssParam, gParams, '=');
				std::getline(ssParam, value, '=');

				std::istringstream ssgParams(gParams);
				std::string group;
				std::string parameter;
				std::getline(ssgParams, group, '.');
				std::getline(ssgParams, parameter, '.');

				// Update Parameter
				OnChangeParameter(group, parameter, value);

				std::string html = MakeRedirect("/");
				(void)client.Send(html.c_str(), html.size());
			}
			else
			{
				std::string html = Make404();
				(void)client.Send(html.c_str(), html.size());
			}
		}

		void CParameterServer::OnRequestUser(const IO::CSocket& client, const std::string& url)
		{
			//printf("Remote : %s  -- %s\n", IO::SocketUtility::addrToIP(client.GetAddressRemote().addr).c_str(), IO::SocketUtility::addrToIP(client.GetAddressBind().addr).c_str());

			std::string page;
			std::string updateValue;
			std::istringstream ss(url);
			std::getline(ss, page, ' ');
			std::getline(ss, updateValue, ' ');

			if (page == "list")
			{
				ParameterPacket packet;

				for (const auto& group : m_params)
				{
					for (const auto& paramSet : group.second)
					{
						ParameterObject obj;
						obj.GroupName = group.first;
						obj.Name = paramSet.first;
						obj.Type = paramSet.second->ToStringType();
						obj.Value = paramSet.second->ToStringValue();
						packet.parameters.emplace_back(obj);
					}
				}

				CSerializer serializer;
				serializer << packet;

				RoutineMessageHeader Header;
				Header.MessageID = 0;
				Header.MessageLength = static_cast<uint32_t>(serializer.size());

				(void)client.Send(&Header, sizeof(RoutineMessageHeader));
				(void)client.Send(serializer.data(), serializer.size());
			}
			else if (page == "update")
			{
				std::istringstream ssParam(updateValue);
				std::string params;
				std::string value;

				std::getline(ssParam, params, '=');
				std::getline(ssParam, value, '=');

				std::istringstream ssParamDetail(params);
				std::string groupParams;
				std::string nameParams;

				std::getline(ssParamDetail, groupParams, '.');
				std::getline(ssParamDetail, nameParams, '.');

				// Update Parameter
				OnChangeParameter(groupParams, nameParams, value);
			}
			else if (page == "save")
			{
				SaveParameter();
			}
		}

		void CParameterServer::OnChangeParameter(const std::string& group, const std::string& parameter, std::string value)
		{
			if (m_params.find(group) != m_params.end())
			{
				if (m_params[group].find(parameter) != m_params[group].end())
				{
					m_params[group][parameter]->SetValue(std::move(value));
				}
				else
				{
					printf("Not Register Parameter\n");
				}
			}
			else
			{
				printf("Not Register Parameter\n");
			}
		}

		std::string CParameterServer::MakeJSon() const
		{
			std::string ret;
			struct cJSON* json = cJSON_CreateObject();
			if (json)
			{
				for (const auto& group : m_params)
				{
					cJSON* itemGroup = cJSON_CreateObject();
					cJSON_AddItemToObject(json, group.first.c_str(), itemGroup);

					for (const auto& param : group.second)
					{
						if (param.second->ToStringType() == "bool_t")
						{
							cJSON* itemValue = cJSON_CreateString(param.second->ToStringValue().c_str());
							cJSON_AddItemToObject(itemGroup, param.first.c_str(), itemValue);
						}
						else if (param.second->ToStringType() == "float_t" || param.second->ToStringType() == "double_t")
						{
							const auto value = reinterpret_cast<CParamValue<double_t>*>(param.second.get());
							cJSON* itemValue = cJSON_CreateNumber(value->data());
							cJSON_AddItemToObject(itemGroup, param.first.c_str(), itemValue);
						}
						else if (param.second->ToStringType() == "uint8_t")
						{
							const auto value = reinterpret_cast<CParamValue<uint8_t>*>(param.second.get());
							cJSON* itemValue = cJSON_CreateNumber(value->data());
							cJSON_AddItemToObject(itemGroup, param.first.c_str(), itemValue);
						}
						else if (param.second->ToStringType() == "uint16_t")
						{
							const auto value = reinterpret_cast<CParamValue<uint16_t>*>(param.second.get());
							cJSON* itemValue = cJSON_CreateNumber(value->data());
							cJSON_AddItemToObject(itemGroup, param.first.c_str(), itemValue);
						}
						else if (param.second->ToStringType() == "uint32_t")
						{
							const auto value = reinterpret_cast<CParamValue<uint32_t>*>(param.second.get());
							cJSON* itemValue = cJSON_CreateNumber(value->data());
							cJSON_AddItemToObject(itemGroup, param.first.c_str(), itemValue);
						}
						else if (param.second->ToStringType() == "uint64_t")
						{
							const auto value = reinterpret_cast<CParamValue<uint64_t>*>(param.second.get());
							cJSON* itemValue = cJSON_CreateNumber(static_cast<double_t>(value->data()));
							cJSON_AddItemToObject(itemGroup, param.first.c_str(), itemValue);
						}
						else if (param.second->ToStringType() == "size_t")
						{
							const auto value = reinterpret_cast<CParamValue<size_t>*>(param.second.get());
							cJSON* itemValue = cJSON_CreateNumber(static_cast<double_t>(value->data()));
							cJSON_AddItemToObject(itemGroup, param.first.c_str(), itemValue);
						}
						else if (param.second->ToStringType() == "int8_t")
						{
							const auto value = reinterpret_cast<CParamValue<int8_t>*>(param.second.get());
							cJSON* itemValue = cJSON_CreateNumber(value->data());
							cJSON_AddItemToObject(itemGroup, param.first.c_str(), itemValue);
						}
						else if (param.second->ToStringType() == "int16_t")
						{
							const auto value = reinterpret_cast<CParamValue<int16_t>*>(param.second.get());
							cJSON* itemValue = cJSON_CreateNumber(value->data());
							cJSON_AddItemToObject(itemGroup, param.first.c_str(), itemValue);
						}
						else if (param.second->ToStringType() == "int32_t")
						{
							const auto value = reinterpret_cast<CParamValue<int32_t>*>(param.second.get());
							cJSON* itemValue = cJSON_CreateNumber(value->data());
							cJSON_AddItemToObject(itemGroup, param.first.c_str(), itemValue);
						}
						else if (param.second->ToStringType() == "int64_t")
						{
							const auto value = reinterpret_cast<CParamValue<int64_t>*>(param.second.get());
							cJSON* itemValue = cJSON_CreateNumber(static_cast<double_t>(value->data()));
							cJSON_AddItemToObject(itemGroup, param.first.c_str(), itemValue);
						}
						else if (param.second->ToStringType() == "string_t")
						{
							const auto value = reinterpret_cast<CParamValue<string_t>*>(param.second.get());
							cJSON* itemValue = cJSON_CreateString(value->data().c_str());
							cJSON_AddItemToObject(itemGroup, param.first.c_str(), itemValue);
						}
					}
				}

				ret = cJSON_Print(json);
				cJSON_Delete(json);
			}

			return ret;
		}

		std::string CParameterServer::MakeHTTP(uint32_t code, const std::string& html, const std::string& contentType) const
		{
			std::string status;
			switch (code)
			{
			case 200:
				status = "200 OK\r\n";
				break;

			case 404:
			default:
				status = "404 Not Found\r\n";
				break;
			}

			const std::string header = "HTTP/1.1 " + status + "Content-Length: " + std::to_string(html.length()) + "\r\n" + "Content-Type: " + contentType + "\r\nConnection: Keep-Alive\r\nKeep-Alive: max=20, timeout=120\r\n\r\n";

			return header + html;
		}

		std::string CParameterServer::Make404() const
		{
			const std::string nameExecute = GetProcessName();

			const std::string html = "<!doctype html>\
			<html><head><title>" + nameExecute + " Parameter" + "</title></head>\
			<body>\
			<h1>404 Not Found.</h1><p>"\
				"</body> </html>";

			return MakeHTTP(404, html);
		}

		std::string CParameterServer::MakeParameterContents() const
		{
			std::string contents;

			for (const auto& group : m_params)
			{
				contents += "<hr>";
				contents += "<h2>" + group.first + "</h2>";

				for (const auto& paramSet : group.second)
				{
					contents += R"(<form method = "get" action ="change">)";
					contents += "<b>" + paramSet.first + "(" + paramSet.second->ToStringType() + ")" + " : </b>";


					if (paramSet.second->ToStringType() == "bool_t")
					{
						if (paramSet.second->ToStringValue() == "true")
						{
							contents += R"(<label><input type = "radio" name = ")" + group.first + '.' + paramSet.first +
								R"(" value = "true" checked> true </label>)";
							contents += R"(<label><input type = "radio" name = ")" + group.first + '.' + paramSet.first +
								R"(" value = "false"> false </label>)";
							contents += R"(<input type ="submit" value ="Update Parameter">)";
						}
						else
						{
							contents += R"(<label><input type = "radio" name = ")" + group.first + '.' + paramSet.first +
								R"(" value = "true"> true </label>)";
							contents += R"(<label><input type = "radio" name = ")" + group.first + '.' + paramSet.first +
								R"(" value = "false" checked> false </label>)";
							contents += R"(<input type ="submit" value ="Update Parameter">)";
						}
					}
					else if (paramSet.second->ToStringType() == "float_t" || paramSet.second->ToStringType() == "double_t")
					{
						contents += R"(<label><input style="text-align:right" type = "number" step="0.000000001" name = ")"
							+ group.first + '.' + paramSet.first + "\" value = \"" + paramSet.second->ToStringValue() + "\" checked></label>";
						contents += R"(<input type ="submit" value ="Update Parameter">)";
					}
					else if (paramSet.second->ToStringType() == "uint8_t")
					{
						contents += R"(<label><input style="text-align:right" type = "number" min="0" max="255" name = ")" + group.first + '.' + paramSet.first + "\" value = \"" + paramSet.second->ToStringValue() + "\" checked></label>";
						contents += R"(<input type ="submit" value ="Update Parameter">)";
					}
					else if (paramSet.second->ToStringType() == "uint16_t")
					{
						contents += R"(<label><input style="text-align:right" type = "number" min="0" max="65535" name = ")"
							+ group.first + '.' + paramSet.first + "\" value = \"" + paramSet.second->ToStringValue() + "\" checked></label>";
						contents += R"(<input type ="submit" value ="Update Parameter">)";
					}
					else if (paramSet.second->ToStringType() == "uint32_t")
					{
						contents +=
							R"(<label><input style="text-align:right" type = "number" min="0" max="4294967295" name = ")" + group.first + '.' + paramSet.first + "\" value = \"" + paramSet.second->ToStringValue() + "\" checked></label>";
						contents += R"(<input type ="submit" value ="Update Parameter">)";
					}
					else if (paramSet.second->ToStringType() == "uint64_t")
					{
						contents +=
							R"(<label><input style="text-align:right" type = "number" min="0" max="18446744073709551615" name = ")"
							+ group.first + '.' + paramSet.first + "\" value = \"" + paramSet.second->ToStringValue() + "\" checked></label>";
						contents += R"(<input type ="submit" value ="Update Parameter">)";
					}
					else if (paramSet.second->ToStringType() == "int8_t")
					{
						contents +=
							R"(<label><input style="text-align:right" type = "number" min="-128" max="127" name = ")" + group.first + '.' + paramSet.first + "\" value = \"" + paramSet.second->ToStringValue() + "\" checked></label>";
						contents += R"(<input type ="submit" value ="Update Parameter">)";
					}
					else if (paramSet.second->ToStringType() == "int16_t")
					{
						contents +=
							R"(<label><input style="text-align:right" type = "number" min="-32768 " max="32767" name = ")" + group.first + '.' + paramSet.first + "\" value = \"" + paramSet.second->ToStringValue() + "\" checked></label>";
						contents += R"(<input type ="submit" value ="Update Parameter">)";
					}
					else if (paramSet.second->ToStringType() == "int32_t")
					{
						contents +=
							R"(<label><input style="text-align:right" type = "number" min="-2147483648" max="2147483647" name = ")"
							+ group.first + '.' + paramSet.first + "\" value = \"" + paramSet.second->ToStringValue() + "\" checked></label>";
						contents += R"(<input type ="submit" value ="Update Parameter">)";
					}
					else if (paramSet.second->ToStringType() == "int64_t")
					{
						contents +=
							R"(<label><input style="text-align:right" type = "number" min="-9223372036854775808" max="9223372036854775807" name = ")"
							+ group.first + '.' + paramSet.first + "\" value = \"" + paramSet.second->ToStringValue() + "\" checked></label>";
						contents += R"(<input type ="submit" value ="Update Parameter">)";
					}
					else if (paramSet.second->ToStringType() == "string_t")
					{
						contents +=
							R"(<label><input style="text-align:right" type = "text" name = ")"
							+ group.first + '.' + paramSet.first + "\" value = \"" + paramSet.second->ToStringValue() + "\" checked></label>";
						contents += R"(<input type ="submit" value ="Update Parameter">)";
					}
					else if (paramSet.second->ToStringType() == "unknown")
					{
					}
					contents += R"(</form>)";
					contents += R"(<br>)";
				}
			}

			contents += "<hr>";
			contents += "<h2>Save Config File.</h2>";
			contents += R"(<form method = "get" action ="save">)";
			contents += R"(<input type ="submit" value ="Save )" + GetMakeConfigName() + R"(">)";
			contents += R"(</form>)";

			return contents;
		}

		std::string CParameterServer::MakeMain() const
		{
			const std::string nameExecute = GetProcessName();
			const std::string contents = MakeParameterContents();

			const std::string html = "<!doctype html>\
			<html><head><title>" + nameExecute + " Parameter" + "</title></head>\
			<body>\
			<h1>" + nameExecute + " Parameter Configure." + "</h1><p>"\
				+ contents +
				"</body> </html>";

			return MakeHTTP(200, html);
		}

		std::string CParameterServer::MakeRedirect(const std::string& hostAddr) const
		{
			const std::string html = "<!doctype html>\
			<html> <meta http-equiv=\"refresh\" content=\"0; url = " + hostAddr + "\"></meta></html>";

			return MakeHTTP(200, html);
		}

		//void CParameterServer::ParserCommand(IO::CSocket& client, std::string strCommand)
		//{

		//}

		//void CParameterServer::OnReceiveData(IO::CSocket& client, const std::string& ip, uint16_t port, void_ptr pData, int32_t size)
		//{
		//	const std::string command(static_cast<char*>(pData));
		//	m_bufferCommand += command;

		//	auto end = static_cast<int32_t>(m_bufferCommand.find("\r\n\r\n"));

		//	while ( end > 0 )
		//	{
		//		const std::string request = m_bufferCommand.substr(0, end);
		//		m_bufferCommand.erase(m_bufferCommand.begin(), m_bufferCommand.begin() + end + 4);
		//		ParserCommand(client, request);

		//		end = static_cast<int32_t>(m_bufferCommand.find("\r\n\r\n"));
		//	}
		//}
	}
}