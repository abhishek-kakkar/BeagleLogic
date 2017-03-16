/**
* beaglelogictestapp.c
*
* Copyright (C) 2014 Kumar Abhishek
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License version 2 as
* published by the Free Software Foundation.
*
* 8Mhz sample rate 16 bit samples
*/

#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/poll.h>
#include <sys/errno.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <unistd.h>
#include "seniorDesign/seniorDesignLib.h"
#include "libbeaglelogic.h"

int bfd, i;

/* Globals to Keep Track Of */
int pub_signal = 0;
int transmit = 1;
uint32_t forwardCount[5] = {0};
uint32_t backwardCount[5] = {0};
uint32_t errorCount[5] = {0};
uint32_t risingEdgeCounts[10] = {0};
uint32_t LastRisingEdgeTime[10] = {0};
uint32_t clockValue = 0;
uint32_t event = 9999;
uint8_t *buf,*bl_mem;
pthread_t MQTT_t;
sem_t MQTT_mutex;

/* For testing nonblocking IO */
#define NONBLOCK

/* for prover stroke */
#define proverStart 0b00100000
#define proverEnd   0b00010000
#define proverMask  0b00110000

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

	exit(-1);
}

/* Handles SIGSEGV */
void segfaulthandler(int x)
{
	printf("segfault caught at i = %X\n", i);

	fflush(stdout);
	beaglelogic_close(bfd);

	if (buf)
		free(buf);

	exit(-1);
}

/* Handles itimer */
void timer_handler(int signum) {
	/* Set Flag */
	pub_signal = 1;
}

int main(int argc, char **argv)
{
	int cnt1;
	size_t sz, sz_to_read, cnt;

	/*buffer for read*/
	char buffer[4 * 1000 * 1000];
	struct timespec t1, t2, t3, t4;
	struct pollfd pollfd;
	struct sigaction sa;
	struct itimerval timer;
	MQTT_Package package_t;

	/* Init Sempahore */
	sem_init(&MQTT_mutex, 0, 0);


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

	/* Install timer handler */
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = &timer_handler;
	sigaction(SIGVTALRM, &sa, NULL);

	/* Configure timer to expire after .5 sec */
	timer.it_value.tv_sec = 0;
	timer.it_value.tv_usec = 500000;
	/* Create the interval with the same time */
	timer.it_interval.tv_sec = 0;
	timer.it_interval.tv_usec = 500000;
	/* Start a .5 timer that increase the MQTT_mutex semaphore */
	setitimer(ITIMER_VIRTUAL, &timer, NULL);


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
	beaglelogic_set_samplerate(bfd, 8 * 1000 * 1000);
	beaglelogic_set_sampleunit(bfd, BL_SAMPLEUNIT_16_BITS);
	beaglelogic_set_triggerflags(bfd, BL_TRIGGERFLAGS_CONTINUOUS);
	clock_gettime(CLOCK_MONOTONIC, &t2);
	printf("Configured in %jd us\n", timediff(&t1, &t2));

	/* All set, start a capture */
	beaglelogic_start(bfd);

	/* Spawn MQTT thread */
	package_t.bfd_cpy = bfd;
	package_t.pollfd = pollfd;
	package_t.MQTT_mutex = &MQTT_mutex;
	if (start_MQTT_t(&package_t, MQTT_t)) {
		return 1;
	}

	clock_gettime(CLOCK_MONOTONIC, &t1);
	cnt = 0;
	for (i = 0; i < 10; i++) {

		/* Configure counters */
		cnt1 = 0;

#if defined(NONBLOCK)
		poll(&pollfd, 1, 500);
		int i;
		while (1) {

			/*Start a timer for Debug */
			clock_gettime(CLOCK_MONOTONIC, &t3);

			sz = read(bfd, buffer, 4*1000*1000);

			/*Check For bit changes*/
			for (i = 0; i < 4 * 1000 * 1000; i+=2) {

				/*Debug*/
				//printf("%2x %2x\n", buffer[i], buffer[i + 1]);

				/* incremeant our time */
				clockValue++;

				/* Check past with present values */
				if (buffer[i] != buffer[i-2] || buffer[i + 1] != buffer[i-1]){
					changeState((int) buffer[i], (int) buffer[i + 1]);
				}

				/* check to see if we need to transmit to MQTT*/
				if (pub_signal){

					/* Update event */
					event = 0;
					MQTT_queueData(&package_t);
				}
				else if(buffer[i+1] & proverMask == proverStart){

					/* Update event */
					event = 1;
					MQTT_queueData(&package_t);
				}
				else if(buffer[i] & proverMask == proverEnd){

					/* Update event */
					event = 2;
					MQTT_queueData(&package_t);
				}
			}

			/* Debug timer */
			clock_gettime(CLOCK_MONOTONIC, &t4);
			printf("time for read and process = %jd\n", timediff(&t3,&t4));

			if (sz == 0)
				break;
			else if (sz == -1) {
				poll(&pollfd, 1, 500);
				continue;
			}

			cnt1 += sz;
		}
#else
		(void)pollfd;
		do {
			sz = read(bfd, buf2, 64 * 1024 * 16);
			if (sz == -1)
				break;
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
