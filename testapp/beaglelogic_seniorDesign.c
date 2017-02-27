/* d
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
#include <pthread.h>

#include <time.h>
#include <unistd.h>

#include "seniorDesign/lfq.h"
#include "seniorDesign/seniorDesignLib.h"
#include "libbeaglelogic.h"

int bfd, i;
uint8_t *buf,*bl_mem;

pthread_t process_t;
pthread_t MQTT_t;

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
	size_t sz_to_read, cnt;

	struct timespec t1, t2;
	struct pollfd pollfd;
	seniorDesignPackage package_t;
	struct lfq circleBuff;

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
	beaglelogic_set_samplerate(bfd, 45*1000);
	beaglelogic_set_sampleunit(bfd, BL_SAMPLEUNIT_16_BITS);
	beaglelogic_set_triggerflags(bfd, BL_TRIGGERFLAGS_CONTINUOUS);
	clock_gettime(CLOCK_MONOTONIC, &t2);
	printf("Configured in %jd us\n", timediff(&t1, &t2));

	beaglelogic_start(bfd);

	/* All set, start a capture */
	clock_gettime(CLOCK_MONOTONIC, &t1);

	/*Call process and MQTT Thread*/

	/*Initialize lfq*/
	void*  buff_ptr = (void *)malloc(32 * 1000 * 1000 * sizeof(void));
	void** buff_pptr = buff_ptr;

	lfq_init(&circleBuff, 32 * 1000 * 1000, buff_pptr);
#if defined(NONBLOCK)
	/*Start Threads*/
	package_t.ptr_lfq = &circleBuff;
	package_t.bfd_cpy = bfd;
	package_t.pollfd = pollfd;

	/*
	if(start_process_t(&package_t, process_thread)){
		return 1;
	}

//	if(start_MQTT_t(&package_t, MQTT_thread)){
//		return 1;
//	}

	/*Join Threads*/
	//pthread_join(process_t, NULL);
	//pthread_join(MQTT_t, NULL);
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
	clock_gettime(CLOCK_MONOTONIC, &t2);

	printf("Read %d bytes in %jd us, speed=%jd MB/s\n",
			cnt, timediff(&t1, &t2), cnt / timediff(&t1, &t2));

	/* Done, close mappings, file and free the buffers */
	beaglelogic_munmap(bfd, bl_mem);
	beaglelogic_close(bfd);

	free(buf);

	return 0;
}
