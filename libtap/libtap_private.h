#ifndef __LIBTAP_PRIVATE_H__
#define __LIBTAP_PRIVATE_H__

#include <net/if.h>

struct knet_tap {
        struct ifreq ifr;
        int knet_tap_fd;
};

#endif