/*
 * Copyright (C) 2010-2015 Red Hat, Inc.  All rights reserved.
 *
 * Authors: Fabio M. Di Nitto <fabbione@kronosnet.org>
 *          Federico Simoncelli <fsimon@kronosnet.org>
 *
 * This software licensed under GPL-2.0+, LGPL-2.0+
 */

#ifndef __KNET_THREADS_COMMON_H__
#define __KNET_THREADS_COMMON_H__

#include "internals.h"

#define KNET_THREADS_TIMERES 200000

#define timespec_diff(start, end, diff) \
do { \
	if (end.tv_sec > start.tv_sec) \
		*(diff) = ((end.tv_sec - start.tv_sec) * 1000000000llu) \
					+ end.tv_nsec - start.tv_nsec; \
	else \
		*(diff) = end.tv_nsec - start.tv_nsec; \
} while (0);

int shutdown_in_progress(knet_handle_t knet_h);

#endif
