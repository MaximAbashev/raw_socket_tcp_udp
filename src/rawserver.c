# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <netinet/in.h>
# include <sys/socket.h>
# include <sys/types.h>
# include <linux/if_ether.h>
# include <arpa/inet.h>
# include "func.h"

# define PCKT_LEN 1024
# define SRV_PORT 4444
# define CLNT_PORT 5555

void main ()
{
	int raw_sock, from_clnt_buf, len;
	const int on = 1;
	struct sockaddr_in s_addr, clnt_addr;
	char *datagram;
	struct udp_header *udp_hdr;
	struct ipv4_header *ip_hdr;
/*
 * Fabricate socket and set socket options.
 */
	s_addr.sin_family = AF_INET;
	s_addr.sin_port = htons (SRV_PORT);
	s_addr.sin_addr.s_addr = htonl (INADDR_LOOPBACK);
	raw_sock = socket (AF_INET, SOCK_RAW, IPPROTO_UDP);
	if (raw_sock < 0)
	{
		perror ("Open socket error");
		exit (1);
	}
	len = sizeof (struct sockaddr_in); /* for recvfrom */
	while (1)
	{
		while (2)
		{
			datagram = malloc (sizeof (char) * PCKT_LEN);
			bzero (datagram, PCKT_LEN);
			from_clnt_buf = recvfrom (raw_sock, datagram, PCKT_LEN, 0, 
                    (struct sockaddr *)&clnt_addr, &len);
			if (from_clnt_buf > 0)
			{
				printf ("I take a message... this is...\n");
/*
 * My packet have ttl 70
 */				
                ip_hdr = (struct ipv4_header*)(datagram);
				if ((ip_hdr -> ttl) == 70)
				{
					printf ("Frendly message!\n");
				}
				else
				{
					printf ("Enemy message!\n");
				}
/*
 * Print UDP packet part.
 */    
				udp_hdr = 
                  (struct udp_header*)(datagram + sizeof (struct ipv4_header));				
				printf ("IP_HDR -> IHL: %d, %.2x\n", ip_hdr -> ihl, 
                    ip_hdr -> ihl);
				printf ("IP_HDR -> VERSION: %d, %.2x\n", ip_hdr -> version, 
                    ip_hdr -> version);
				printf ("IP_HDR -> TOS: %d, %.2x\n", ip_hdr -> tos, 
                    ip_hdr -> tos);
				printf ("IP_HDR -> TOT_LEN: %d, %.2x\n", ip_hdr -> tot_len, 
                    ip_hdr -> tot_len);
				printf ("IP_HDR -> ID: %d, %.2x\n", ip_hdr -> id, 
                    ip_hdr -> id);
				printf ("IP_HDR -> FRAG_OFF: %d, %.2x\n", ip_hdr -> frag_off, 
                    ip_hdr -> frag_off);
				printf ("IP_HDR -> TTL: %d, %.2x\n", ip_hdr -> ttl, 
                    ip_hdr -> ttl);
				printf ("IP_HDR -> PROTOCOL: %d, %.2x\n", ip_hdr -> protocol, 
                    ip_hdr -> protocol);
				printf ("IP_HDR -> CHECK: %d, %.2x\n", ip_hdr -> check, 
                    ip_hdr -> check);
				printf ("IP_HDR -> SADDR: %d, %.2x\n", ip_hdr -> saddr, 
                    ip_hdr -> saddr);
				printf ("IP_HDR -> DADDR: %d, %.2x\n", ip_hdr -> daddr, 
                    ip_hdr -> daddr);
				printf ("-------------------------------------------------\n");
				free (datagram);
				break;
			}
			perror ("Packet receive error");
		}
	}
}
