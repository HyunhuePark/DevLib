#pragma once

#include "../Base/DevLibTypes.hpp"

namespace DevLib {
	namespace IO {

#ifdef _WIN64
		using socket_t = int64_t;
#else	// _WIN32
		using socket_t = int32_t;
#endif // _WIN64

		using sharedSocket = std::shared_ptr<socket_t>;

		// Socket Types
		constexpr int32_t SOCK_TYPE_TCP = 1;
		constexpr int32_t SOCK_TYPE_UDP = 2;
		constexpr int32_t SOCK_TYPE_RAW = 3;
		constexpr int32_t SOCK_TYPE_RAW_UDP = 0xF0;
		constexpr int32_t SOCK_TYPE_RAW_ICMP = 0xF1;

		constexpr int32_t PROTOCOL_IP = 0;
		constexpr int32_t PROTOCOL_ICMP = 1;
		constexpr int32_t PROTOCOL_IGMP = 2;
		constexpr int32_t PROTOCOL_TCP = 6;
		constexpr int32_t PROTOCOL_UDP = 17;

		/* ICMP types */
		constexpr int32_t ICMP_ECHOREPLY = 0;	/* ICMP type: echo reply */
		constexpr int32_t ICMP_ECHOREQ = 8;	/* ICMP type: echo request */

		using SOCK_ADDR = struct sock_addr
		{
			uint16_t	family;
			uint16_t	port;
			union {
				struct { uint8_t s_b1, s_b2, s_b3, s_b4; } S_un_b;
				struct { uint16_t s_w1, s_w2; } S_un_w;
				uint32_t addr;
			};
			uint8_t		zero[8]; // Padding 8Byte
		};		// Need 16Byte

		using IPV4_HDR = struct ip_hdr
		{
			uint8_t		ip_header_len : 4;	// 4-bit header length (in 32-bit words) normally=5 (Means 20 Bytes may be 24 also)
			uint8_t		ip_version : 4;	// 4-bit IPv4 version
			uint8_t		ip_tos;			// IP type of service
			uint16_t	ip_total_length; // Total length
			uint16_t	ip_id;			// Unique identifier

			uint8_t		ip_frag_offset : 5; // Fragment offset field

			uint8_t		ip_more_fragment : 1;
			uint8_t		ip_dont_fragment : 1;
			uint8_t		ip_reserved_zero : 1;

			uint8_t		ip_frag_offset1; //fragment offset

			uint8_t		ip_ttl;			// Time to live
			uint8_t		ip_protocol;		// Protocol(TCP,UDP etc)
			uint16_t	ip_checksum;		// IP checksum
			uint32_t	ip_srcaddr;		// Source address
			uint32_t	ip_destaddr;		// Source address
		};

		using UDP_HDR = struct udp_hdr
		{
			uint16_t	source_port;		// Source port no.
			uint16_t	dest_port;		// Dest. port no.
			uint16_t	udp_length;		// Udp packet length
			uint16_t	udp_checksum;	// Udp checksum (optional)
		};

		// TCP header
		using TCP_HDR = struct tcp_header
		{
			uint16_t	source_port;		// source port
			uint16_t	dest_port;		// destination port
			uint32_t	sequence;			// sequence number - 32 bits
			uint32_t	acknowledge;		// acknowledgement number - 32 bits

			uint8_t		ns : 1;			//Nonce Sum Flag Added in RFC 3540.
			uint8_t		reserved_part1 : 3; //according to rfc
			uint8_t		data_offset : 4; /*The number of 32-bit words in the TCP header.
												This indicates where the frameQueue begins.
												The length of the TCP header is always a multiple
												of 32 bits.*/

			uint8_t fin : 1; //Finish Flag
			uint8_t syn : 1; //Synchronise Flag
			uint8_t rst : 1; //Reset Flag
			uint8_t psh : 1; //Push Flag
			uint8_t ack : 1; //Acknowledgement Flag
			uint8_t urg : 1; //Urgent Flag

			uint8_t ecn : 1; //ECN-Echo Flag
			uint8_t cwr : 1; //Congestion Window Reduced Flag

			////////////////////////////////

			uint16_t window;			// window
			uint16_t checksum;		// checksum
			uint16_t urgent_pointer;	// urgent pointer
		};

		using ICMP_HDR = struct icmp_hdr
		{
			uint32_t type;	// ICMP Error type
			uint32_t code;	// Type sub code
			uint16_t checksum;
			uint16_t id;
			uint16_t seq;
		};

	}
}