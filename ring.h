#ifndef __RING_H__
#define __RING_H__

#include <stdint.h>
#include <netinet/in.h>

typedef struct knet_handle *knet_handle_t;

#define KNET_RING_DEFPORT 50000
#define KNET_RING_RCVBUFF 8192

#define KNET_MAX_HOST_LEN 64

struct knet_host {
	uint8_t node_id;
	char name[KNET_MAX_HOST_LEN];
	unsigned int active:1; /* data packets are sent to all links */
	struct knet_link *link;
	struct knet_host *next;
};

struct knet_link {
	int sock;
	struct sockaddr_storage address;
	unsigned int enabled:1;	/* link is enabled for data */
	suseconds_t latency; /* average latency computed by fix/exp */
	unsigned int latency_exp;
	unsigned int latency_fix;
	suseconds_t ping_interval;
	suseconds_t pong_timeout;
	struct timespec ping_last;
	struct timespec pong_last;
	struct knet_link *next;
};

struct knet_listener {
	int sock;
	struct sockaddr_storage address;
	struct knet_listener *next;
};

#define KNET_FRAME_MAGIC 0x12344321
#define KNET_FRAME_VERSION 0x01

#define KNET_FRAME_DATA 0x00
#define KNET_FRAME_PING 0x01
#define KNET_FRAME_PONG 0x02

struct knet_frame {
	uint32_t magic;
	uint8_t	version;
	uint8_t type;
	uint16_t __pad;
} __attribute__((packed));

knet_handle_t knet_handle_new(void);

int knet_handle_getfd(knet_handle_t knet_h);

int knet_host_acquire(knet_handle_t knet_h, struct knet_host **head, int writelock);
int knet_host_release(knet_handle_t knet_h);
int knet_host_add(knet_handle_t khandle, struct knet_host *host);
int knet_host_remove(knet_handle_t khandle, struct knet_host *host);

void knet_link_timeout(struct knet_link *lnk, time_t interval, time_t timeout, int precision);

int knet_listener_acquire(knet_handle_t knet_h, struct knet_listener **head, int writelock);
int knet_listener_release(knet_handle_t knet_h);
int knet_listener_add(knet_handle_t knet_h, struct knet_listener *listener);
int knet_listener_remove(knet_handle_t knet_h, struct knet_listener *listener);

#endif
