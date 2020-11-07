#include <stdio.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <errno.h>

#include "altid.h"
#include "mdns.h"

/* scan.c - adapted from https://github.com/mjansson/mdns/blob/master/mdns.c

	Scan for any altid entries
	List them by common name, and internally we can resolve DNS to that name
	using our ._tcp._altid or whichever namespace

*/

#define MAX_ENTRIES 64;
static char addrbuffer[64];
static char entrybuffer[256];
static char namebuffer[256];

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

static bool
isaltid(char* record)
{
	if(strstr(record, "altid") != 0)
		return true;

	return false;
}

static int
scancb(int sock, const struct sockaddr* from, size_t addrlen, mdns_entry_type_t entry, uint16_t query_id, uint16_t rtype, uint16_t rclass, uint32_t ttl, const void* data, size_t size, size_t name_offset, size_t name_length, size_t record_offset, size_t record_length, void* user_data) {
	(void)sizeof(sock);
	(void)sizeof(query_id);
	(void)sizeof(name_length);
	(void)sizeof(entrybuffer);

	/* Bail early */
	if (rtype != MDNS_RECORDTYPE_PTR)
		return 0;

	Service *service, *list;
	mdns_string_t addrstr;

	list = *(Service**)user_data;
	addrstr = ip_address_to_string(addrbuffer, sizeof(addrbuffer), from, addrlen);
	mdns_record_parse_ptr(data, size, record_offset, record_length, namebuffer, sizeof(namebuffer));
	printf("%s\n", namebuffer);

	/* Check if we have an Altid entry */
	if(!isaltid(namebuffer))
		return 0;

	/* If we have zero entries */
	if(list->isfirst){
		sprintf(list->name, "%s", namebuffer);
		sprintf(list->addr, "%.*s", MDNS_STRING_FORMAT(addrstr));
		list->isfirst = false;
		list->next = NULL;

		return 1;
	}

	/* Kind of messy, but it gets the job did */
	service = malloc(sizeof(Service));
	sprintf(service->name, "%s", list->name);
	sprintf(service->addr, "%s", list->addr);
	service->next = NULL;
	service->isfirst = false;

	/* scrub out any entry if we're connected to it */
	//if(isconnected(service)){
	//	free(service);
	//	return 0;
	//}
	
	sprintf(list->name, "%s", namebuffer);
	sprintf(list->addr, "%.*s", MDNS_STRING_FORMAT(addrstr));

	/* Walk to the end */
	for(Service *si = list; si; si = si->next)
		if(si->next == NULL){
			si->next = service;
			return 1;
		}

	return 1;
}

void *
initmdns(void *arg){
	Service *list;

	list = *(Service**)arg;
	scanmdns(&list);

	return NULL;
}

void
scanmdns(Service **service)
{
	Service *svc;
	int sockets[32];
	int nsocks;
	size_t capacity = 2048;
	void* buffer;
	size_t records;
	int res;

	svc = *service;
	svc->isfirst = true;
	svc->next = NULL;

	nsocks = opensockets(sockets, sizeof(sockets) / sizeof(sockets[0]), 0);

	if(nsocks <= 0)
		return;

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
						&svc
					);
	} while (res > 0);

	free(buffer);

	for(int i = 0; i < nsocks; i++)
		mdns_socket_close(sockets[i]);

	return;
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
