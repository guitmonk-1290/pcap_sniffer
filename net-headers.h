// Header file containing header for the IP, TCP and Ethernet layer

#define ETHER_HDR_LEN 14
#define ETHER_ADDR_LEN 6

// Header for Ethernet packet
struct ether_hdr {
  unsigned char ether_dest_addr[ETHER_ADDR_LEN];    // destination MAC address
  unsigned char ether_src_addr[ETHER_ADDR_LEN];     // source MAC address
  unsigned short ether_type;                        // Ethernet packet type
}

// Header for IP packet
struct ip_hdr {
	unsigned char ip_ver_and_header_len;	// ver. and header length
	unsigned char ip_tos;			// type of service
	unsigned short ip_len;			// total length
	unsigned short ip_id;			// identification no.
	unsigned short ip_frag_off;		// fragment offset, flags
	unsigned char ip_ttl;			// time to live
	unsigned char ip_type;			// protocol time
	unsigned short ip_checksum;		// Checksum
	unsigned int ip_src_addr;		// source IP address
	unsigned int ip_dest_addr;		// destination IP address
};

// Header for TCP packet
struct tcp_hdr {
	unsigned short tcp_src_port;		// source tcp port
	unsigned short tcp_dest_port;		// destination tcp port
	unsigned int tcp_seq;			// tcp sequence number
	unsigned int tcp_ack;			// TCP acknowledgement no.
	unsigned char reserved:4;		// 4bits from 6bits reserved space
	unsigned char tcp_offset:4;		// tcp data offset ...
	unsigned char tcp_flags;		// TCP flags
#define TCP_FIN 0x01
#define TCP_SYN 0x02
#define TCP_RST 0x04
#define TCP_PUSH 0x08
#define TCP_ACK 0x10
#define TCP_URG 0x20
	unsigned short tcp_window;		// TCP window size
	unsigned short tcp_checksum;		// tcp checksum
	unsigned short tcp_urgent;		// tcp urgent pointer
};
