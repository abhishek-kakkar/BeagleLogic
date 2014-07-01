/*
 * Userspace syscall wrapper for BeagleLogic
 * includes ioctl commands and enumeration of states
 *
 * Copyright (C) 2014 Kumar Abhishek <abhishek@theembeddedkitchen.net>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <fcntl.h>

#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>

#include <unistd.h>

#include <stdint.h>
#include <stdlib.h>

#include "libbeaglelogic.h"

/* BeagleLogic device node name */
#define BEAGLELOGIC_DEV_NODE        "/dev/beaglelogic"
#define BEAGLELOGIC_SYSFS_ATTR(a)   "/sys/devices/virtual/misc/beaglelogic/"\
                                      __STRING(a)

/* ioctl calls that can be issued on /dev/beaglelogic */
#define IOCTL_BL_GET_VERSION        _IOR('k', 0x20, uint32_t)

#define IOCTL_BL_GET_SAMPLE_RATE    _IOR('k', 0x21, uint32_t)
#define IOCTL_BL_SET_SAMPLE_RATE    _IOW('k', 0x21, uint32_t)

#define IOCTL_BL_GET_SAMPLE_UNIT    _IOR('k', 0x22, uint32_t)
#define IOCTL_BL_SET_SAMPLE_UNIT    _IOW('k', 0x22, uint32_t)

#define IOCTL_BL_GET_TRIGGER_FLAGS  _IOR('k', 0x23, uint32_t)
#define IOCTL_BL_SET_TRIGGER_FLAGS  _IOW('k', 0x23, uint32_t)

#define IOCTL_BL_CACHE_INVALIDATE    _IO('k', 0x25)

#define IOCTL_BL_GET_BUFFER_SIZE    _IOR('k', 0x26, uint32_t)
#define IOCTL_BL_SET_BUFFER_SIZE    _IOW('k', 0x26, uint32_t)

#define IOCTL_BL_GET_BUFUNIT_SIZE   _IOR('k', 0x27, uint32_t)

#define IOCTL_BL_FILL_TEST_PATTERN   _IO('k', 0x28)

#define IOCTL_BL_START               _IO('k', 0x29)
#define IOCTL_BL_STOP                _IO('k', 0x2A)

int beaglelogic_open(void) {
	return open(BEAGLELOGIC_DEV_NODE, O_RDONLY);
}

int beaglelogic_open_nonblock(void) {
	return open(BEAGLELOGIC_DEV_NODE, O_RDONLY | O_NONBLOCK);
}

int beaglelogic_close(int fd) {
	return close(fd);
}

/* Read from the BeagleLogic file */
int beaglelogic_read(int fd, void *buf, size_t bytes) {
	return read(fd, buf, bytes);
}

int beaglelogic_get_buffersize(int fd, uint32_t *bufsize) {
	return ioctl(fd, IOCTL_BL_GET_BUFFER_SIZE, bufsize);
}

int beaglelogic_set_buffersize(int fd, uint32_t bufsize) {
	return ioctl(fd, IOCTL_BL_SET_BUFFER_SIZE, bufsize);
}

int beaglelogic_get_samplerate(int fd, uint32_t *samplerate) {
	return ioctl(fd, IOCTL_BL_GET_SAMPLE_RATE, samplerate);
}

int beaglelogic_set_samplerate(int fd, uint32_t samplerate) {
	return ioctl(fd, IOCTL_BL_SET_SAMPLE_RATE, samplerate);
}

int beaglelogic_get_sampleunit(int fd,
		enum beaglelogic_sampleunit *sampleunit) {
	return ioctl(fd, IOCTL_BL_GET_SAMPLE_UNIT, sampleunit);
}

int beaglelogic_set_sampleunit(int fd,
		enum beaglelogic_sampleunit sampleunit) {
	return ioctl(fd, IOCTL_BL_SET_SAMPLE_UNIT, sampleunit);
}

int beaglelogic_get_triggerflags(int fd,
		enum beaglelogic_triggerflags *triggerflags) {
	return ioctl(fd, IOCTL_BL_GET_TRIGGER_FLAGS, triggerflags);
}

int beaglelogic_set_triggerflags(int fd,
		enum beaglelogic_triggerflags triggerflags) {
	return ioctl(fd, IOCTL_BL_SET_TRIGGER_FLAGS, triggerflags);
}

int beaglelogic_getlasterror(void) {
	int fd = open(BEAGLELOGIC_SYSFS_ATTR(lasterror), O_RDONLY);
	char buf[16];
	char *endptr;
	int lasterror, ret;

	if (!fd)
		return -1;

	if ((ret = read(fd, buf, 16)) < 0)
		return -1;

	close(fd);
	lasterror = strtoul(buf, &endptr, 10);

	return lasterror;
}

int beaglelogic_start(int fd) {
	return ioctl(fd, IOCTL_BL_START);
}

int beaglelogic_stop(int fd) {
	return ioctl(fd, IOCTL_BL_STOP);
}

int beaglelogic_memcacheinvalidate(int fd) {
	return ioctl(fd, IOCTL_BL_CACHE_INVALIDATE);
}

int beaglelogic_getbufunitsize(int fd) {
	size_t sz;
	ioctl(fd, IOCTL_BL_GET_BUFUNIT_SIZE, &sz);
	return sz;
}

void * beaglelogic_mmap(int fd) {
	size_t sz;
	void *addr;

	if (beaglelogic_get_buffersize(fd, &sz))
		return MAP_FAILED;

	addr = mmap(NULL, sz, PROT_READ, MAP_SHARED, fd, 0);

	return addr;
}

int beaglelogic_munmap(int fd, void *addr) {
	size_t sz;

	if (beaglelogic_get_buffersize(fd, &sz))
		return -1;

	return munmap(addr, sz);
}

int beaglelogic_waitfornextbuffer(void) {
	int fd = open(BEAGLELOGIC_SYSFS_ATTR(state), O_RDONLY);
	char buf[16];
	char *endptr;
	int nextbuf, ret;

	if (!fd)
		return -1;

	if ((ret = read(fd, buf, 16)) < 0)
		return -1;

	close(fd);
	nextbuf = strtoul(buf, &endptr, 10);

	return nextbuf;
}
