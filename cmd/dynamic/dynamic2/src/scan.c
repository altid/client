#include <stdio.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <errno.h>

#include "altid.h"
#include "../resources/mdns.h"

/* scan.c - adapted from https://github.com/mjansson/mdns/blob/master/mdns.c

	Scan for any altid entries
	List them by common name, and internally we can resolve DNS to that name
	using our ._tcp._altid or whichever namespace

*/

#define MAX_ENTRIES 64;

static char addrbuffer[64];
static char entrybuffer[256];
static char namebuffer[256];
static char sendbuffer[256];
static mdns_record_txt_t txtbuffer[128];

static int opensockets(int*, int, int);
static uint32_t service_address;

static mdns_string_t
ipv4_address_to_string(char* buffer, size_t capacity, const struct sockaddr_in* addr,
                       size_t addrlen) {
	char host[NI_MAXHOST] = {0};
	char service[NI_MAXSERV] = {0};
	int ret = getnameinfo((const struct sockaddr*)addr, (socklen_t)addrlen, host, NI_MAXHOST,
	                      service, NI_MAXSERV, NI_NUMERICSERV | NI_NUMERICHOST);
	int len = 0;
	if (ret == 0) {
		if (addr->sin_port != 0)
			len = snprintf(buffer, capacity, "%s:%s", host, service);
		else
			len = snprintf(buffer, capacity, "%s", host);
	}
	if (len >= (int)capacity)
		len = (int)capacity - 1;
	mdns_string_t str;
	str.str = buffer;
	str.length = len;
	return str;
}

static mdns_string_t
ipv6_address_to_string(char* buffer, size_t capacity, const struct sockaddr_in6* addr,
                       size_t addrlen) {
	char host[NI_MAXHOST] = {0};
	char service[NI_MAXSERV] = {0};
	int ret = getnameinfo((const struct sockaddr*)addr, (socklen_t)addrlen, host, NI_MAXHOST,
	                      service, NI_MAXSERV, NI_NUMERICSERV | NI_NUMERICHOST);
	int len = 0;
	if (ret == 0) {
		if (addr->sin6_port != 0)
			len = snprintf(buffer, capacity, "[%s]:%s", host, service);
		else
			len = snprintf(buffer, capacity, "%s", host);
	}
	if (len >= (int)capacity)
		len = (int)capacity - 1;
	mdns_string_t str;
	str.str = buffer;
	str.length = len;
	return str;
}

static mdns_string_t
ip_address_to_string(char* buffer, size_t capacity, const struct sockaddr* addr, size_t addrlen) {
	if (addr->sa_family == AF_INET6)
		return ipv6_address_to_string(buffer, capacity, (const struct sockaddr_in6*)addr, addrlen);
	return ipv4_address_to_string(buffer, capacity, (const struct sockaddr_in*)addr, addrlen);
}

static int
scancb(int sock, const struct sockaddr* from, size_t addrlen, mdns_entry_type_t entry, uint16_t query_id, uint16_t rtype, uint16_t rclass, uint32_t ttl, const void* data, size_t size, size_t name_offset, size_t name_length, size_t record_offset, size_t record_length, void* user_data) {
	(void)sizeof(sock);
	(void)sizeof(query_id);
	(void)sizeof(name_length);

	// user_data is our channel we send entries down on
	(void)sizeof(user_data);

	mdns_string_t fromaddrstr = ip_address_to_string(addrbuffer, sizeof(addrbuffer), from, addrlen);
	const char* entrytype = (entry == MDNS_ENTRYTYPE_ANSWER) ?
	                            "answer" :
	                            ((entry == MDNS_ENTRYTYPE_AUTHORITY) ? "authority" : "additional");
	mdns_string_t entrystr =
	    mdns_string_extract(data, size, &name_offset, entrybuffer, sizeof(entrybuffer));
	if (rtype == MDNS_RECORDTYPE_PTR) {
		mdns_string_t namestr = mdns_record_parse_ptr(data, size, record_offset, record_length,
		                                              namebuffer, sizeof(namebuffer));
		printf("%.*s : %s %.*s PTR %.*s rclass 0x%x ttl %u length %d\n",
		       MDNS_STRING_FORMAT(fromaddrstr), entrytype, MDNS_STRING_FORMAT(entrystr),
		       MDNS_STRING_FORMAT(namestr), rclass, ttl, (int)record_length);
	} else if (rtype == MDNS_RECORDTYPE_SRV) {
		mdns_record_srv_t srv = mdns_record_parse_srv(data, size, record_offset, record_length,
		                                              namebuffer, sizeof(namebuffer));
		printf("%.*s : %s %.*s SRV %.*s priority %d weight %d port %d\n",
		       MDNS_STRING_FORMAT(fromaddrstr), entrytype, MDNS_STRING_FORMAT(entrystr),
		       MDNS_STRING_FORMAT(srv.name), srv.priority, srv.weight, srv.port);
	} else if (rtype == MDNS_RECORDTYPE_A) {
		struct sockaddr_in addr;
		mdns_record_parse_a(data, size, record_offset, record_length, &addr);
		mdns_string_t addrstr =
		    ipv4_address_to_string(namebuffer, sizeof(namebuffer), &addr, sizeof(addr));
		printf("%.*s : %s %.*s A %.*s\n", MDNS_STRING_FORMAT(fromaddrstr), entrytype,
		       MDNS_STRING_FORMAT(entrystr), MDNS_STRING_FORMAT(addrstr));
	} else if (rtype == MDNS_RECORDTYPE_AAAA) {
		struct sockaddr_in6 addr;
		mdns_record_parse_aaaa(data, size, record_offset, record_length, &addr);
		mdns_string_t addrstr =
		    ipv6_address_to_string(namebuffer, sizeof(namebuffer), &addr, sizeof(addr));
		printf("%.*s : %s %.*s AAAA %.*s\n", MDNS_STRING_FORMAT(fromaddrstr), entrytype,
		       MDNS_STRING_FORMAT(entrystr), MDNS_STRING_FORMAT(addrstr));
	} else if (rtype == MDNS_RECORDTYPE_TXT) {
		size_t parsed = mdns_record_parse_txt(data, size, record_offset, record_length, txtbuffer,
		                                      sizeof(txtbuffer) / sizeof(mdns_record_txt_t));
		for (size_t itxt = 0; itxt < parsed; ++itxt) {
			if (txtbuffer[itxt].value.length) {
				printf("%.*s : %s %.*s TXT %.*s = %.*s\n", MDNS_STRING_FORMAT(fromaddrstr),
				       entrytype, MDNS_STRING_FORMAT(entrystr),
				       MDNS_STRING_FORMAT(txtbuffer[itxt].key),
				       MDNS_STRING_FORMAT(txtbuffer[itxt].value));
			} else {
				printf("%.*s : %s %.*s TXT %.*s\n", MDNS_STRING_FORMAT(fromaddrstr), entrytype,
				       MDNS_STRING_FORMAT(entrystr), MDNS_STRING_FORMAT(txtbuffer[itxt].key));
			}
		}
	} else {
		printf("%.*s : %s %.*s type %u rclass 0x%x ttl %u length %d\n",
		       MDNS_STRING_FORMAT(fromaddrstr), entrytype, MDNS_STRING_FORMAT(entrystr), rtype,
		       rclass, ttl, (int)record_length);
	}
	return 0;
}

int
scanmdns(DNSData* results)
{
	int sockets[32];
	int nsocks;

	size_t capacity = 2048;
	void* buffer;
	void* user_data = 0;
	size_t records;

	int res;

	nsocks = opensockets(sockets, sizeof(sockets) / sizeof(sockets[0]), 0);

	if(nsocks <= 0)
		return ENOTCONN;

	for(int i = 0; i < nsocks; i++)
		mdns_discovery_send(sockets[i]);
			
	buffer = malloc(capacity);
	
	do {
		int nfds = 0;
		struct timeval timeout;
		fd_set readfs;

		timeout.tv_sec = 5;
		timeout.tv_usec = 0;

		FD_ZERO(&readfs);
		for(int i = 0; i < nsocks; i++){
			if(sockets[i] >= nfds)
				nfds = sockets[i] + 1;
			FD_SET(sockets[i], &readfs);
		}

		records = 0;
		res = select(nfds, &readfs, 0, 0, &timeout);
		
		if(res > 0)
			for(int i = 0; i < nsocks; i++)
				if(FD_ISSET(sockets[i], &readfs))
					records += mdns_discovery_recv(
						sockets[i],
						buffer,
						capacity,
						scancb,
					    user_data
					);
	} while (res > 0);

	free(buffer);

	for(int i = 0; i < nsocks; i++)
		mdns_socket_close(sockets[i]);

	return 0;
}
	
static int
opensockets(int* sockets, int max_sockets, int port)
{
	int nsocks = 0;
	int first_addr = 1;

	struct ifaddrs* ifaddr = 0;
	struct ifaddrs* ifa = 0;

	getifaddrs(&ifaddr);
	
	for(ifa = ifaddr; ifa; ifa = ifa->ifa_next){
		if(ifa->ifa_addr == 0)
			continue;
		
		/* IPv4 for now */
		if(ifa->ifa_addr->sa_family == AF_INET){
			struct sockaddr_in* saddr = (struct sockaddr_in*)ifa->ifa_addr;
			if (saddr->sin_addr.s_addr != htonl(INADDR_LOOPBACK)) {

				if (first_addr) {
					service_address = saddr->sin_addr.s_addr;
					first_addr = 0;
				}
				
				if (nsocks < max_sockets) {
					saddr->sin_port = htons(port);
					int sock = mdns_socket_open_ipv4(saddr);
					if (sock >= 0)
						sockets[nsocks++] = sock;
				}
			}
		}
	}

	freeifaddrs(ifaddr);
	return nsocks;
}
