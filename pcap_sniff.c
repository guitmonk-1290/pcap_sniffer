#include <pcap.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "net-headers.h"

void pcap_fatal(const char* failed_in, const char* errbuf) {
  printf("Fatal error in %s: %s\n", failed_in, errbuf);
  exit(1);
}

// callback function for pcap_loop
void caught_packet(u_char* usr_args, const struct pcap_pkthdr* cap_header, const u_char packet) {
  int tcp_header_len, total_header_len, pkt_data_len;
  u_char *pkt_data;
  
  printf("=== Got %d byte packet ===\n", cap_header->len);
  
  decode_ethernet(packet);
  decode_ip(packet+ETHER_HDR_LEN);
  tcp_header_len = decode_tcp(packet+ETHER_HDR_LEN+sizeof(struct ip_hdr));
  
  total_header_size = ETHER_HDR_LEN + sizeof(struct ip_hdr) + tcp_header_len;
  pkt_data = (u_char *)packet + total_header_size;
  pkt_data_len = cap_header->len - total_header_size;
  if(pkt_data_len > 0) {
    printf("\t\t\t%u bytes of packet data\n", pkt_data_len);
		dump(pkt_data, pkt_data_len);
  }
  else {
    printf(""\t\t\tNo Packet Data\n);
  }
}

// decode ethernet header
void decode_ethernet(const u_char* header_start) {
	int i;
	const struct ether_hdr* ethernet_header;

	ethernet_header = (const struct ether_hdr *)header_start;
	printf("[[ Layer 2 :: Ethernet Header ]]\n");
	printf("[ Source: %02x", ethernet_header->ether_src_addr);
	for(i=1;i<ETHER_ADDR_LEN;i++) {
		printf(":%02x", ethernet_header->ether_src_addr[i]);
	}
	printf("[\tDest: %02x", ethernet_header->ether_dest_addr[0]);
	for(i=1;i<ETHER_ADDR_LEN;i++) {
		printf(":%02x", ethernet_header->ether_dest_addr[i]);
	}
	printf("\tType: %hu ]\n", ethernet_header->ether_type);
}

// decode IP header
void decode_ip(const u_char* header_start) {
	const struct ip_hdr* ip_header;
	ip_header = (const struct* ip_hdr)header_start;
	printf("\t(( Layer 3 ::: IP Header ))\n");
	printf("\t ( Source: %s\t", inet_ntoa(ip_header->ip_src_addr));
	printf("Dest: %s )\n", inet_ntoa(ip_header->ip_dest_addr));
	printf("\t( Type: %u\t", (u_int)ip_header->ip_type);
	printf("ID: %hu\tLength: %hu )\n", ntohs(ip_header->ip_id), ntohs(ip_header->ip_len));
}

// decode TCP header
u_int decode_tcp(const u_char* header_start) {
	u_int header_size;
	const struct tcp_hdr tcp_header;

	tcp_header = (const struct tcp_hdr*)header_start;
	header_size = tcp_header->tcp_offset;

	printf("\t\t{{ Layer 4 :::: TCP Header }}\n");
	printf("\t\t{ Src Port: %hu\t", ntohs(tcp_header->tcp_src_port));
	printf("Dest Port: %hu }\n", ntohs(tcp_header->tcp_dest_port));
	printf("\t\t{ Seq #: %u\t", ntohl(tcp_header->tcp_seq));
	printf("Ack #: %u }\n", ntohl(tcp_header->tcp_ack));
	printf("\t\t{ Header Size: %u\tFlags: ", header_size);
	if(tcp_header->tcp_flags & TCP_FIN) printf("FIN ");
	if(tcp_header->tcp_flags & TCP_SYN) printf("SYN ");
	if(tcp_header->tcp_flags & TCP_RST) printf("RST ");
	if(tcp_header->tcp_flags & TCP_PUSH) printf("PUSH ");
	if(tcp_header->tcp_flags & TCP_ACK) printf("ACK ");
	if(tcp_header->tcp_flags & TCP_URG) printf("URG ");
	printf("\n");

	return header_size;
}

int main() {
  struct pcap_pkthdr header;
  const u_char* packet;
  char* device;
  char errbuf[PCAP_ERRBUF_SIZE];
  pcap_t* pcap_handle;
  int i;
  
  device = pcap_lookupdev(errbuf);
  if(device == NULL) {
    pcap_fatal("pacp_lookupdev", errbuf);
  }
  
  printf("sniffing on device: %s\n", device);
  
  pcap_handle = pcap_open_live(device, 4096, 1, 0, errbuf);
  if(pcap_handle == NULL) {
    pcap_fatal("pcap_open_live", errbuf);
  }
  pcap_loop(pcap_handle, 3, caught_packet, NULL);
  pcap_close(pcap_handle);
}
