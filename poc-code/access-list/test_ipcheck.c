#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <malloc.h>
#include "ipcheck.h"

/* This is a test program .. remember! */
#define BUFLEN 1024

static int get_ipaddress(char *buf, struct sockaddr_storage *addr)
{
	struct addrinfo *info;
	struct addrinfo hints;
	int res;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;

	res = getaddrinfo(buf, NULL, &hints, &info);
	if (!res) {
		memmove(addr, info->ai_addr, info->ai_addrlen);
		free(info);
	}
	return res;
}

static int read_address(char *buf, struct sockaddr_storage *addr)
{
	return get_ipaddress(buf, addr);
}

static int read_mask(char *buf, struct sockaddr_storage *addr, struct sockaddr_storage *addr2)
{
	char tmpbuf[BUFLEN];
	char *slash;
	int ret;

	slash = strchr(buf, '/');
	if (!slash)
		return 1;

	strncpy(tmpbuf, buf, slash-buf);
	tmpbuf[slash-buf] = '\0';

	ret = get_ipaddress(tmpbuf, addr);
        if (ret)
		return ret;

	ret = get_ipaddress(slash+1, addr2);
        if (ret)
		return ret;

	return 0;
}

static int read_range(char *buf, struct sockaddr_storage *addr1, struct sockaddr_storage *addr2)
{
	char tmpbuf[BUFLEN];
	char *hyphen;
	int ret;

	hyphen = strchr(buf, '-');
	if (!hyphen)
		return 1;

	strncpy(tmpbuf, buf, hyphen-buf);
	tmpbuf[hyphen-buf] = '\0';

	ret = get_ipaddress(tmpbuf, addr1);
        if (ret)
		return ret;

	ret = get_ipaddress(hyphen+1, addr2);
        if (ret)
		return ret;

	return 0;
}


static int load_file(void)
{
	FILE *filterfile;
	char filebuf[BUFLEN];
	int line = 0;
	int ret;
	ipcheck_type_t type;
	ipcheck_acceptreject_t acceptreject;
	struct sockaddr_storage addr1;
	struct sockaddr_storage addr2;

	ipcheck_clear();

	filterfile = fopen("test_ipcheck.txt", "r");
	if (!filterfile) {
		fprintf(stderr, "Cannot open test_ipcheck.txt\n");
		return 1;
	}

	while (fgets(filebuf, sizeof(filebuf), filterfile)) {
		filebuf[strlen(filebuf)-1] = '\0'; /* remove trailing LF */
		line++;

		/*
		 * First char is A (accept) or R (Reject)
		 */
		switch(filebuf[0] & 0x5F) {
		case 'A':
			acceptreject = IPCHECK_ACCEPT;
			break;
		case 'R':
			acceptreject = IPCHECK_REJECT;
			break;
		default:
			fprintf(stderr, "Unknown record type on line %d: %s\n", line, filebuf);
			goto next_record;
		}

		/*
		 * Second char is the filter type:
		 * A Address
		 * M Mask
		 * R Range
		 */
		switch(filebuf[1] & 0x5F) {
		case 'A':
			type = IPCHECK_TYPE_ADDRESS;
			ret = read_address(filebuf+2, &addr1);
			break;
		case 'M':
			type = IPCHECK_TYPE_MASK;
			ret = read_mask(filebuf+2, &addr1, &addr2);
			break;
		case 'R':
			type = IPCHECK_TYPE_RANGE;
			ret = read_range(filebuf+2, &addr1, &addr2);
			break;
		default:
			fprintf(stderr, "Unknown filter type on line %d: %s\n", line, filebuf);
			goto next_record;
			break;
		}
		if (ret) {
			fprintf(stderr, "Failed to parse address on line %d: %s\n", line, filebuf);
		}
		else {
			ipcheck_addip(&addr1, &addr2, type, acceptreject);
		}
	next_record: {} /* empty statement to mollify the compiler */
	}
	fclose(filterfile);

	return 0;
}

int main(int argc, char *argv[])
{
	struct sockaddr_storage saddr;
	int ret;
	int i;

	if (load_file())
		return 1;

	for (i=1; i<argc; i++) {
		ret = get_ipaddress(argv[i], &saddr);
		if (ret) {
			fprintf(stderr, "Cannot parse address %s\n", argv[i]);
		}
		else {
			if (ipcheck_validate(&saddr)) {
				printf("%s is VALID\n", argv[i]);
			}
			else {
				printf("%s is not allowed\n", argv[i]);
			}
		}
	}

	return 0;
}
