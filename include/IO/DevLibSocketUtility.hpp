#pragma once

#include "DevLibSocketProtocol.hpp"
#include "CSocket.hpp"

#if defined(_MSC_VER)
#pragma warning(disable : 5105)
#include <WinSock2.h>
#include <ws2ipdef.h>
#include <WS2tcpip.h>
#include <iphlpapi.h>
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "IPHLPAPI.lib")
#pragma warning(default : 5105)
#elif defined(__linux__) // Linux
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <sys/epoll.h>
#endif

namespace DevLib
{
	namespace IO
	{
#if defined(_MSC_VER)
#elif defined(__linux__) // Linux
		constexpr int ADDR_ANY = 0;
		constexpr int SOCKET_ERROR = -1;
#endif

		namespace SocketUtility
		{
#if defined(_MSC_VER)
			static bool _g_socketInitCount_ = false;
			static bool InitWinSock()
			{
				bool bRet = false;
				WSAData wsaData{};

				if (_g_socketInitCount_ == false)
				{
					if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
					{
						//print32_tf("SOCKET Init Error\n");
					}
					else
					{
						_g_socketInitCount_ = true;
						bRet = true;
					}
				}

				return bRet;
			}

			static void UnInitWinSock()
			{
				if (_g_socketInitCount_ == true)
				{
					WSACleanup();
				}
			}
#elif defined(__linux__) // Linux

#endif

			inline std::string addrToIP(const uint32_t _addr, const int32_t family = AF_INET)
			{
				sockaddr_in addr{};
				memset(&addr, 0, sizeof addr);

				addr.sin_addr.s_addr = _addr;

				char buff[32] = "";
				inet_ntop(family, &addr.sin_addr, buff, 32);

				return {buff};
			}

			inline std::string addrToIP(const SOCK_ADDR _addr)
			{
				char buff[32] = "";
				inet_ntop(_addr.family, &_addr.addr, buff, 32);

				return {buff};
			}

			inline uint16_t addrToPort(const SOCK_ADDR _addr)
			{
				return htons(_addr.port);
			}

			inline uint32_t ipToAddr(const std::string &_ip, const int32_t family = AF_INET)
			{
				uint32_t addr = 0;
				inet_pton(family, _ip.c_str(), &addr);

				return addr;
			}

			inline IPV4_HDR *GetIPHeader(void *rawPacketData)
			{
				return static_cast<IPV4_HDR *>(rawPacketData);
			}

			inline TCP_HDR *GetTCPHeader(void *rawPacketData)
			{
				return reinterpret_cast<TCP_HDR *>(static_cast<uint8_t *>(rawPacketData) + GetIPHeader(rawPacketData)->ip_header_len * 4);
			}

			inline UDP_HDR *GetUDPHeader(void *rawPacketData)
			{
				return reinterpret_cast<UDP_HDR *>(static_cast<uint8_t *>(rawPacketData) + GetIPHeader(rawPacketData)->ip_header_len * 4);
			}

			inline ICMP_HDR *GetICMPHeader(void *rawPacketData)
			{
				return reinterpret_cast<ICMP_HDR *>(static_cast<uint8_t *>(rawPacketData) + GetIPHeader(rawPacketData)->ip_header_len * 4);
			}

			inline std::string GetPacketSourceIP(void *rawPacketData)
			{
				return addrToIP(static_cast<IPV4_HDR *>(rawPacketData)->ip_srcaddr);
			}

			inline std::string GetPacketDestinationIP(void *rawPacketData)
			{
				return addrToIP(static_cast<IPV4_HDR *>(rawPacketData)->ip_destaddr);
			}

			inline uint32_t GetPacketProtocolType(void *rawPacketData)
			{
				return static_cast<IPV4_HDR *>(rawPacketData)->ip_protocol;
			}

			inline uint16_t GetMakeChecksum(void *pData, int nbytes)
			{
				auto *ptr = static_cast<uint16_t *>(pData);
				uint32_t sum = 0;
				uint16_t oddbyte;

				while (nbytes > 1)
				{
					sum += *ptr++;
					nbytes -= 2;
				}

				if (nbytes == 1)
				{
					oddbyte = 0;
					*reinterpret_cast<u_char *>(&oddbyte) = *reinterpret_cast<u_char *>(ptr);
					sum += oddbyte;
				}

				sum = (sum >> 16) + (sum & 0xffff);
				sum += sum >> 16;
				const auto answer = static_cast<uint16_t>(~sum);

				return answer;
			}

			inline bool IsConnected(CSocket &socket)
			{
				bool bRet = false;

				if (socket.GetSocketHandle())
				{
					char buf;
					const bool oldMode = socket.SetSockBlockIO(false);
					const int nRead = recv(socket.GetSocketHandle(), &buf, 1, MSG_PEEK);

#if defined(_MSC_VER)
					const int err = WSAGetLastError();
#elif defined(__linux__) // Linux
					const int err = errno;
					constexpr int SOCKET_ERROR = -1;
					constexpr int WSAEWOULDBLOCK = EWOULDBLOCK;
					constexpr int WSAENOTCONN = ENOTCONN;
#endif
					socket.SetSockBlockIO(oldMode);

					if (nRead == SOCKET_ERROR)
					{
						switch (err)
						{
#if defined(_MSC_VER)
							// case EAGAIN: // Windows == WSAEWOULDBLOCK
#endif // Linux
						case WSAEWOULDBLOCK:
						case WSAENOTCONN:
							bRet = false;
							break;
						default:
							bRet = true;
						}
					}
				}

				return bRet;
			}

			/*
			Socket Option
			*/
			inline void SetSockNagle(const socket_t sock, const bool bUse)
			{
				// Socket Option : Nagle ( IPPROTO_TCP )
				// 0 : Nagle Use, 1 : Nagle NoUse
				int32_t option = 1;
				if (bUse)
					option = 0;
				setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<char *>(&option), sizeof option);
			}

			inline void SetSockBlockIO(const socket_t sock, const bool bBlock)
			{
				// If iMode = 0, blocking is enabled;
				// If iMode != 0, non-blocking mode is enabled.
#if defined(_MSC_VER)
				unsigned long option = 1;
				if (bBlock)
					option = 0;
				ioctlsocket(sock, FIONBIO, &option);
#elif defined(__linux__) // Linux
				int flags = fcntl(sock, F_GETFL, 0);

				if (bBlock)
				{
					fcntl(sock, F_SETFL, flags & ~O_NONBLOCK);
				}
				else
				{
					fcntl(sock, F_SETFL, flags | O_NONBLOCK);
				}
#endif
			}

			inline void SetSockReuse(const socket_t sock, const bool bReuse)
			{
				// Socket Option : Reuse Addr ( SOL_SOCKET )
				// SO_REUSEADDR = true
				int32_t reuse_addr = bReuse;
				setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char *>(&reuse_addr), sizeof reuse_addr);
			}

			inline bool SetSockBroadcast(const socket_t sock, const bool bUse)
			{
				bool bRet = false;
				int32_t bcast = 1;
				if (bUse == false)
					bcast = 0;
				if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, reinterpret_cast<char *>(&bcast), sizeof bcast) == 0)
				{
					bRet = true;
				}
				return bRet;
			}

			inline bool SetSockKeepAlive(const socket_t sock, const bool bUse, uint32_t idleSec = 60, uint32_t intervalSec = 1, uint32_t intervalCount = 10)
			{
				bool bRet = false;
				int32_t bKeepAlive = 0;

				if (bUse == true)
					bKeepAlive = 1;

				// SOL_TCP 6  ==> PROTOCOL_TCP
				if (setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, reinterpret_cast<char *>(&bKeepAlive), sizeof bKeepAlive) == 0)
				{
					if (setsockopt(sock, PROTOCOL_TCP, TCP_KEEPIDLE, reinterpret_cast<char *>(&idleSec), sizeof idleSec) == 0)
					{
						if (setsockopt(sock, PROTOCOL_TCP, TCP_KEEPINTVL, reinterpret_cast<char *>(&intervalSec), sizeof intervalSec) == 0)
						{
							if (setsockopt(sock, PROTOCOL_TCP, TCP_KEEPCNT, reinterpret_cast<char *>(&intervalCount), sizeof intervalCount) == 0)
							{
								bRet = true;
							}
						}
					}
				}
				return bRet;
			}

			inline bool SetSockRecvAll(const socket_t sock, bool bRecvAll)
			{
#if defined(_MSC_VER)
				int32_t in = 0;

#define SIO_RCVALL _WSAIOW(IOC_VENDOR, 1) // this removes the need of mstcpip.h
				return WSAIoctl(sock, SIO_RCVALL, &bRecvAll, sizeof bRecvAll, nullptr, 0, reinterpret_cast<LPDWORD>(&in), nullptr, nullptr) == SOCKET_ERROR ? false : true;
#elif defined(__linux__) // Linux
				return false;
#endif
			}

			inline bool SetTimeoutRecv(const socket_t sock, uint32_t ms)
			{
				struct timeval timeout;
				timeout.tv_sec = ms / 1000;           
				timeout.tv_usec = (ms % 1000) * 1000; 

				return setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char*>(&timeout), sizeof(timeout)) == 0 ? true : false; // read timeout
			}

			inline bool SetTimeoutSend(const socket_t sock, uint32_t ms)
			{
				struct timeval timeout;
				timeout.tv_sec = ms / 1000;
				timeout.tv_usec = (ms % 1000) * 1000;

				return setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<const char*>(&timeout), sizeof(timeout)) == 0 ? true : false; // send timeout
			}

	
			inline uint32_t GetTimeoutRecv(const socket_t sock)
			{
				struct timeval timeout = {};
				socklen_t len = sizeof(timeout);

				getsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<char*>(&timeout), &len);

				return static_cast<uint32_t>(timeout.tv_sec * 1000 + timeout.tv_usec * 0.001);
			}

			inline uint32_t GetTimeoutSend(const socket_t sock)
			{
				struct timeval timeout = {};
				socklen_t len = sizeof(timeout);

				getsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<char*>(&timeout), &len);

				return static_cast<uint32_t>(timeout.tv_sec * 1000 + timeout.tv_usec * 0.001);
			}


			/*
			Multicast Option
			*/

			inline bool MulticastTTL(const socket_t sock, int32_t nTTL = 1)
			{
				bool bRet = false;

				if (sock == SOCK_DGRAM)
				{
					if (setsockopt(sock, IPPROTO_IP, IP_MULTICAST_TTL, reinterpret_cast<char *>(&nTTL), sizeof nTTL) == 0)
					{
						bRet = true;
					}
				}

				return bRet;
			}

			inline bool MulticastJoin(const socket_t sock, const std::string &groupIP, const std::string &bindIP /*= std::string()*/)
			{
				bool bRet = false;
				ip_mreq group{};
				memset(&group, 0, sizeof(ip_mreq));
#if defined(_MSC_VER)
				group.imr_multiaddr.S_un.S_addr = ipToAddr(groupIP);

				if (bindIP.empty())
				{
					group.imr_interface.S_un.S_addr = htonl(INADDR_ANY);
				}
				else
				{
					group.imr_interface.S_un.S_addr = ipToAddr(bindIP);
				}
#elif defined(__linux__) // Linux
				group.imr_multiaddr.s_addr = ipToAddr(groupIP);

				if (bindIP.empty())
				{
					group.imr_interface.s_addr = htonl(INADDR_ANY);
				}
				else
				{
					group.imr_interface.s_addr = ipToAddr(bindIP);
				}
#endif

				if (setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, reinterpret_cast<char *>(&group), sizeof group) == 0)
				{
					bRet = true;
				}

				return bRet;
			}

			inline bool MulticastLeave(const socket_t sock, const std::string &groupIP, const std::string &bindIP /*= std::string()*/)
			{
				bool bRet = false;
				ip_mreq group{};
				memset(&group, 0, sizeof(ip_mreq));
#if defined(_MSC_VER)
				group.imr_multiaddr.S_un.S_addr = ipToAddr(groupIP);

				if (bindIP.empty())
				{
					group.imr_interface.S_un.S_addr = htonl(INADDR_ANY);
				}
				else
				{
					group.imr_interface.S_un.S_addr = ipToAddr(bindIP);
				}

#elif defined(__linux__) // Linux
				group.imr_multiaddr.s_addr = ipToAddr(groupIP);

				if (bindIP.empty())
				{
					group.imr_interface.s_addr = htonl(INADDR_ANY);
				}
				else
				{
					group.imr_interface.s_addr = ipToAddr(bindIP);
				}
#endif

				if (setsockopt(sock, IPPROTO_IP, IP_DROP_MEMBERSHIP, reinterpret_cast<char *>(&group), sizeof group) == 0)
				{
					bRet = true;
				}

				return bRet;
			}

			inline bool MulticastLoopbackEnable(const socket_t sock, const bool bLoopback /*= true*/)
			{
				bool bRet = true;
				const char loopch = bLoopback ? 1 : 0;

				if (setsockopt(sock, IPPROTO_IP, IP_MULTICAST_LOOP, &loopch, sizeof loopch) < 0)
					bRet = false;

				return bRet;
			}

			inline bool MulticastBind(const socket_t sock, const std::string &bindIP)
			{
				bool bRet = true;
				in_addr localInterface{};
				memset(&localInterface, 0, sizeof localInterface);

				localInterface.s_addr = ipToAddr(bindIP);
				if (setsockopt(sock, IPPROTO_IP, IP_MULTICAST_IF, reinterpret_cast<char *>(&localInterface), sizeof localInterface) < 0)
					bRet = false;

				return bRet;
			}

			/*
			ETC Network
			*/
			inline std::map<std::string, std::vector<std::string>> GetInterfaceAddress()
			{
				std::map<std::string, std::vector<std::string>> res;

#ifdef _MSC_VER
#define WORKING_BUFFER_SIZE 15000
#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

				ULONG outBufLen = WORKING_BUFFER_SIZE;
				const auto pAddresses = static_cast<IP_ADAPTER_ADDRESSES*>(MALLOC(outBufLen));
				if (GetAdaptersAddresses(AF_INET, GAA_FLAG_INCLUDE_PREFIX, nullptr, pAddresses, &outBufLen) == NO_ERROR)
				{
					auto pCurrAddresses = pAddresses;
					while (pCurrAddresses) 
					{
						auto pUnicast = pCurrAddresses->FirstUnicastAddress;
						while (pUnicast != nullptr)
						{
#pragma warning(disable : 4244)
							std::wstring str(pCurrAddresses->Description);
							res[std::string(str.begin(), str.end())].push_back(addrToIP(reinterpret_cast<sockaddr_in*>(pUnicast->Address.lpSockaddr)->sin_addr.S_un.S_addr));
							pUnicast = pUnicast->Next;
#pragma warning(default : 4244)
						}

						pCurrAddresses = pCurrAddresses->Next;
					}
				}

#elif defined(__linux__) // Linux
				struct ifaddrs *ifAddrStruct = nullptr;
				struct ifaddrs *ifa = nullptr;
				void *tmpAddrPtr = nullptr;

				getifaddrs(&ifAddrStruct);

				for (ifa = ifAddrStruct; ifa != nullptr; ifa = ifa->ifa_next)
				{
					if (ifa->ifa_addr)
					{
						if (ifa->ifa_addr->sa_family == AF_INET) // check it is IP4
						{
							// is a valid IP4 Address
							tmpAddrPtr = &(reinterpret_cast<struct sockaddr_in *>(ifa->ifa_addr))->sin_addr;
							char addressBuffer[INET_ADDRSTRLEN];
							inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);

							// if (strcmp("127.0.0.1", addressBuffer) != 0) // Skip Loopback
							{
								res[ifa->ifa_name].push_back(std::string(addressBuffer));
							}
						}
					}
				}

				// remove ifaddr
				if (ifAddrStruct)
				{
					freeifaddrs(ifAddrStruct);
				}
#endif

				return res;

			}

			inline std::string HostToIP(const std::string& host)
			{
				std::string ip{};
				struct addrinfo hints = {};
				hints.ai_family = AF_UNSPEC; // IPv4 or IPv6
				hints.ai_socktype = SOCK_STREAM; // Stream socket

				struct addrinfo* result = nullptr;
				const int ret = getaddrinfo(host.c_str(), nullptr, &hints, &result);
				if (ret == 0) 
				{
					for (const struct addrinfo* res = result; res != nullptr; res = res->ai_next) 
					{
						char buff[INET6_ADDRSTRLEN];
						const void* addr = nullptr;

						if (res->ai_family == AF_INET) 
						{
							addr = &reinterpret_cast<struct sockaddr_in*>(res->ai_addr)->sin_addr;
						}
						else if (res->ai_family == AF_INET6) 
						{
							addr = &reinterpret_cast<struct sockaddr_in6*>(res->ai_addr)->sin6_addr;
						}

						if (addr && inet_ntop(res->ai_family, addr, buff, sizeof(buff)) != nullptr) 
						{
							ip = std::string(buff);
							break;
						}
					}
				}
				else
				{
					switch (ret)
					{
					case 10093 :
						ip = "WSANOTINITIALISED";
						break;
					default: ;
					}
				}

				freeaddrinfo(result); // Clean up
				return ip;
			}

		}
	}
}