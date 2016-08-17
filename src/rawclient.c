# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <netinet/in.h>
# include <sys/socket.h>
# include <linux/if_ether.h>
# include <arpa/inet.h>
# include "func.h"

# define PCKT_LEN 1024
# define SRV_PORT 4444
# define CLNT_PORT 5555

void main ()
{
	int raw_sock, from_clnt_buf, i;
	const int on = 1;
	struct sockaddr_in s_addr, clnt_addr;
	char *datagram;
	struct ps_header *ps_hdr;
	struct eth_header *eth_hdr;
	struct ipv4_header *ip_hdr;
	struct udp_header *udp_hdr;
/*
 * Allocate memory for sending packet.
 */
	datagram = malloc (sizeof(char) * PCKT_LEN);
	bzero (datagram, PCKT_LEN);
/*
 * Fabricate socket and set socket options.
 */
	s_addr.sin_family = AF_INET;
	s_addr.sin_addr.s_addr = htonl (INADDR_LOOPBACK);
	s_addr.sin_port = htons (SRV_PORT);
	raw_sock = socket (AF_INET, SOCK_RAW, IPPROTO_RAW);
	if (raw_sock < 0)	
	{
		perror ("Client socket initiate error");
		exit (1);
	}
	if (setsockopt(raw_sock, IPPROTO_IP, IP_HDRINCL, &on, sizeof (on)) < 0)
	{
		perror ("Setsockopt error");
		exit (1);
	}
    while (1)
    {
/* 
 * Fabricate UDP header.
 */
	    udp_hdr = malloc (sizeof (struct udp_header));
	    udp_hdr -> uh_sport = htons (CLNT_PORT);
	    udp_hdr -> uh_dport = s_addr.sin_port;
	    udp_hdr -> uh_ulen = htons (sizeof (struct udp_header) / 4);
	    udp_hdr -> uh_sum = 0x0000;
/* 
 * Fabricate IP header.
 */
	    ip_hdr = malloc (sizeof (struct ipv4_header));
	    ip_hdr -> ihl = 0x5;
	    ip_hdr -> version = 0x4;
	    ip_hdr -> tos = 0x0;
    	ip_hdr -> tot_len = 60;
    	ip_hdr -> id = htons(12830);
    	ip_hdr -> frag_off = 0x0;
    	ip_hdr -> ttl = 70;
    	ip_hdr -> protocol = IPPROTO_UDP;
    	ip_hdr -> check = 0x0;
    	ip_hdr -> saddr = inet_addr("192.168.100.100");
    	ip_hdr -> daddr = s_addr.sin_addr.s_addr;
    	ip_hdr -> check = checksum ((u_short *) ip_hdr, sizeof (*ip_hdr));
    	printf ("IP_HDR -> IHL: %d, %.2x\n", ip_hdr -> ihl, ip_hdr -> ihl);
    	printf ("IP_HDR -> VERSION: %d, %.2x\n", ip_hdr -> version, 
            ip_hdr -> version);
    	printf ("IP_HDR -> TOS: %d, %.2x\n", ip_hdr -> tos, ip_hdr -> tos);
    	printf ("IP_HDR -> TOT_LEN: %d, %.2x\n", ip_hdr -> tot_len, 
            ip_hdr -> tot_len);
    	printf ("IP_HDR -> ID: %d, %.2x\n", ip_hdr -> id, ip_hdr -> id);
    	printf ("IP_HDR -> FRAG_OFF: %d, %.2x\n", ip_hdr -> frag_off, 
            ip_hdr -> frag_off);
    	printf ("IP_HDR -> TTL: %d, %.2x\n", ip_hdr -> ttl, ip_hdr -> ttl);
    	printf ("IP_HDR -> PROTOCOL: %d, %.2x\n", ip_hdr -> protocol, 
            ip_hdr -> protocol);
    	printf ("IP_HDR -> CHECK: %d, %.2x\n", ip_hdr -> check, 
            ip_hdr -> check);
    	printf ("IP_HDR -> SADDR: %d, %.2x\n", ip_hdr -> saddr, 
            ip_hdr -> saddr);
    	printf ("IP_HDR -> DADDR: %d, %.2x\n", ip_hdr -> daddr, 
            ip_hdr -> daddr);
    	printf ("---------------------------------------------------------\n");
/*
 * Compilation packet and sending it.
 */
        memcpy (datagram, ip_hdr, sizeof (*ip_hdr));
    	memcpy (datagram + sizeof (*ip_hdr), udp_hdr, sizeof (*udp_hdr));
    	memcpy (datagram + sizeof (*ip_hdr) + sizeof (*udp_hdr), "Hello\n", 5);
		sleep (2);
		if (sendto (raw_sock, datagram, PCKT_LEN, 0, 
                    (struct sockaddr *)&s_addr, sizeof (s_addr)) < 0)
		{
			perror ("Sendto error!");
			exit (1);
		}	
		free (udp_hdr);
		free (ip_hdr);
	}
	free (datagram);	
	close (raw_sock);
}
