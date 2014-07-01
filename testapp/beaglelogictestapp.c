/*
 * beaglelogictestapp.c
 *
 * Copyright (C) 2014 Kumar Abhishek
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/poll.h>
#include <sys/errno.h>
#include <sys/mman.h>

#include <time.h>
#include <unistd.h>

#include "libbeaglelogic.h"

int bfd, i;
uint8_t *buf, *buf2, *buf3, *bl_mem;

/* For testing nonblocking IO */
#define NONBLOCK

/* Returns time difference in microseconds */
static uint64_t timediff(struct timespec *tv1, struct timespec *tv2)
{
    return ((((uint64_t)(tv2->tv_sec - tv1->tv_sec) * 1000000000) +
        (tv2->tv_nsec - tv1->tv_nsec)) / 1000);
}

/* Handles SIGINT */
void exithandler(int x)
{
	if (buf)
		free(buf);

	printf("sigint caught\n");
	fflush(stdout);
	beaglelogic_close(bfd);

	exit (-1);
}

/* Handles SIGSEGV */
void segfaulthandler(int x)
{
	printf("segfault caught at i = %X\n", i);

	fflush(stdout);
	beaglelogic_close(bfd);

	if (buf)
		free(buf);

	exit (-1);
}

int main(int argc, char **argv)
{
	int cnt1;
	size_t sz, sz_to_read, cnt;

	struct timespec t1, t2;
	struct pollfd pollfd;

	printf("BeagleLogic test application\n");

	/* Open BeagleLogic */
	clock_gettime(CLOCK_MONOTONIC, &t1);
#if defined(NONBLOCK)
	bfd = beaglelogic_open_nonblock();
#else
	bfd = beaglelogic_open();
#endif
	clock_gettime(CLOCK_MONOTONIC, &t2);

	if (bfd == -1) {
		printf("BeagleLogic open failed! \n");
		return -1;
	}

	printf("BeagleLogic opened successfully in %jd us\n",
			timediff(&t1, &t2));

	/* Memory map the file */
	bl_mem = beaglelogic_mmap(bfd);

	/* Configure the poll descriptor */
	pollfd.fd = bfd;
	pollfd.events = POLLIN | POLLRDNORM;

	/* Register signal handlers */
	signal(SIGINT, exithandler);
	signal(SIGSEGV, segfaulthandler);

	/* Configure buffer size - we need a minimum of 32 MB */
	beaglelogic_get_buffersize(bfd, &sz_to_read);
	if (sz_to_read < 32 * 1024 * 1024) {
		beaglelogic_set_buffersize(bfd, sz_to_read = 32 * 1024 * 1024);
		beaglelogic_get_buffersize(bfd, &sz_to_read);
	}
	buf = calloc(sz_to_read / 32, 32);
	memset(buf, 0xFF, sz_to_read);

	printf("Buffer size = %d MB \n", sz_to_read / (1024 * 1024));

	/* Configure capture settings */
	clock_gettime(CLOCK_MONOTONIC, &t1);
	beaglelogic_set_samplerate(bfd, 50 * 1000 * 1000);
	beaglelogic_set_sampleunit(bfd, BL_SAMPLEUNIT_8_BITS);
	beaglelogic_set_triggerflags(bfd, BL_TRIGGERFLAGS_CONTINUOUS);
	clock_gettime(CLOCK_MONOTONIC, &t2);
	printf("Configured in %jd us\n", timediff(&t1, &t2));

	beaglelogic_start(bfd);

	/* All set, start a capture */
	clock_gettime(CLOCK_MONOTONIC, &t1);
	cnt = 0;
	for (i = 0; i < 10; i++) {
		/* Configure counters */
		cnt1 = 0;
		buf2 = buf;
		buf3 = bl_mem;
#if defined(NONBLOCK)
		poll (&pollfd, 1, 500);
		while (cnt1 < sz_to_read && pollfd.revents) {
			/* Do stuff until timeout */
			sz = read(bfd, NULL, 64 * 1024);
			memcpy(buf2, buf3, sz);

			if (sz == 0)
				break;
			else if (sz == -1) {
				poll(&pollfd, 1, 500);
				continue;
			}

			buf2 += sz;
			buf3 += sz;
			cnt1 += sz;
		}
#else
		(void)pollfd;
		do {
			sz = read(bfd, buf2, 64 * 1024 * 16);

			if (sz == -1)
				break;

			buf2 += sz;
			cnt1 += sz;
		} while (sz > 0 && cnt1 < sz_to_read);
#endif
		cnt += cnt1;
	}
	clock_gettime(CLOCK_MONOTONIC, &t2);

	printf("Read %d bytes in %jd us, speed=%jd MB/s\n",
			cnt, timediff(&t1, &t2), cnt / timediff(&t1, &t2));

	/* Done, close mappings, file and free the buffers */
	beaglelogic_munmap(bfd, bl_mem);
	beaglelogic_close(bfd);

	free(buf);

	return 0;
}
