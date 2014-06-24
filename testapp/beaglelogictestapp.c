/*
 * beaglelogictestapp.c
 *
 * Copyright (C) 2014 Kumar Abhishek
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

#include "libbeaglelogic.h"


int main(int argc, char **argv) {
	int bfd, i;
	size_t sz, cnt, sz_to_read;
	uint8_t *buf, *buf2;
	uint8_t *bl_mem;

	sz_to_read = 32 * 1024 * 1024;

	printf("BeagleLogic test applicatiom\n");

	/* Open BeagleLogic */
	bfd = beaglelogic_open();

	if (!bfd) {
		printf("BeagleLogic open failed! \n");
		return -1;
	}

	printf("BeagleLogic opened successfully\n");

	/* Configure buffer size and allocate our private buffer */
	beaglelogic_get_buffersize(bfd, &sz);
	if (sz < 32 * 1024 * 1024) {
		beaglelogic_set_buffersize(bfd, sz = 32 * 1024 * 1024);
		beaglelogic_get_buffersize(bfd, &sz);
	}
	printf("Buffer size = %d MB \n", sz / (1024 * 1024));
	buf = calloc(sz / 64, 64);

	/* Configure capture settings */
	beaglelogic_set_samplerate(bfd, 50 * 1000 * 1000);
	beaglelogic_set_sampleunit(bfd, BL_SAMPLEUNIT_8_BITS);
	beaglelogic_set_triggerflags(bfd, BL_TRIGGERFLAGS_ONESHOT);

	/* All set, start a capture */
	buf2 = buf;
	cnt = 0;
	do {
		sz = beaglelogic_read(bfd, buf2, 64 * 1024);
		if (sz < 0)
			break;

		cnt += sz;
		buf2 += sz;
	} while (sz > 0);

	printf("Read %d bytes\n", cnt);

	beaglelogic_close(bfd);
	free(buf);

	return 0;
}
