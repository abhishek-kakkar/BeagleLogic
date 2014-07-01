/*
 * Userspace API for BeagleLogic
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

#ifndef LIBBEAGLELOGIC_H_
#define LIBBEAGLELOGIC_H_

/* Possible States of BeagleLogic */
enum beaglelogic_states {
	STATE_BL_DISABLED,	/* Powered off (at module start) */
	STATE_BL_INITIALIZED,	/* Powered on */
	STATE_BL_MEMALLOCD,	/* Buffers allocated */
	STATE_BL_ARMED,		/* All Buffers DMA-mapped and configuration done */
	STATE_BL_RUNNING,	/* Data being captured */
	STATE_BL_REQUEST_STOP,	/* Stop requested */
	STATE_BL_ERROR   	/* Buffer overrun */
};

/* Setting attributes */
enum beaglelogic_triggerflags {
	BL_TRIGGERFLAGS_ONESHOT = 0,
	BL_TRIGGERFLAGS_CONTINUOUS
};

/* Possible sample unit / formats */
enum beaglelogic_sampleunit {
	BL_SAMPLEUNIT_16_BITS = 0,
	BL_SAMPLEUNIT_8_BITS
};

/* Open and close functions */
extern int beaglelogic_open(void);
extern int beaglelogic_open_nonblock(void);
extern int beaglelogic_close(int fd);

/* Read from the BeagleLogic file */
int beaglelogic_read(int fd, void *buf, size_t bytes);

/* Gets the unit size of the capture buffer
 * The kernel capture buffer is allocated in multiples of this buffer unit
 * Parameters:
 * 	* fd : The file number to an open /dev/beaglelogic node
 *
 * Returns:
 * 	the unit size
 */
int beaglelogic_getbufunitsize(int fd);

/* Gets and sets the kernel capture buffer size in bytes
 * The buffer allocated may be more than the size requested, hence
 * the program should check the size of the allocated buffer before
 * allocating its private buffers
 *
 * Parameters:
 * 	* fd : The file number to an open /dev/beaglelogic node
 * 	* size : pointer to var (for get) and value (for set)
 * Returns:
 * 	0 on success, -1 on failure
 */
int beaglelogic_get_buffersize(int fd, uint32_t *bufsize);
int beaglelogic_set_buffersize(int fd, uint32_t bufsize);

/* Gets and sets the sample rate (in Hz)
 *
 * Parameters:
 * 	* fd : The file number to an open /dev/beaglelogic node
 * 	* samplerate : pointer to var (for get) and value (for set)
 * Returns:
 * 	0 on success, -1 on failure
 */
int beaglelogic_get_samplerate(int fd, uint32_t *samplerate);
int beaglelogic_set_samplerate(int fd, uint32_t samplerate);

/* Gets and sets the sample unit
 *
 * Parameters:
 * 	* fd : The file number to an open /dev/beaglelogic node
 * 	* sampleunit : pointer to var (for get) and value (for set)
 * Possible values:
 * 	* BL_SAMPLEUNIT_16_BITS : 16-bit samples
 * 	* BL_SAMPLEUNIT_8_BITS : 8-bit samples
 * Returns:
 * 	0 on success, -1 on failure
 */
int beaglelogic_get_sampleunit(int fd, enum beaglelogic_sampleunit *sampleunit);
int beaglelogic_set_sampleunit(int fd, enum beaglelogic_sampleunit sampleunit);

/* Gets and sets the trigger flags
 *
 * Parameters:
 * 	* fd : The file number to an open /dev/beaglelogic node
 * 	* triggerflags : pointer to var (for get) and value (for set)
 * Possible values:
 * 	* BL_TRIGGERFLAGS_ONESHOT : One-shot
 * 	* BL_TRIGGERFLAGS_CONTINUOUS : Continuous
 * Returns:
 * 	0 on success, -1 on failure
 */
int beaglelogic_get_triggerflags(int fd,
		enum beaglelogic_triggerflags *triggerflags);
int beaglelogic_set_triggerflags(int fd,
		enum beaglelogic_triggerflags triggerflags);

/* Polls for last error and returns the error code
 *
 * This function waits till the capture session ends, so may not be
 * called from the same thread that is reading data from the device
 *
 * Returns:
 * 	0 or 0x1nnnn, where nnnn: The buffer on which buffer overrun occurred
 */
int beaglelogic_getlasterror(void);

/* Starts a logic capture
 *
 * This function may be called only while operating with mmap or testing
 * as the logic analyzer is triggered automatically on the first read() call
 *
 * Parameters:
 * 	* fd : The file number to an open /dev/beaglelogic node
 *
 * Returns:
 * 	0 on success, -1 on failure
 */
int beaglelogic_start(int fd);

/* Ends the active capture session
 * Use beaglelogic_getlasterror to busy-wait till the handle is released
 *
 * Parameters:
 * 	* fd : The file number to an open /dev/beaglelogic node
 *
 * Returns:
 * 	0 on success, non-zero indicates some error
 */
int beaglelogic_stop(int fd);

/* Invalidates the kernel buffer cache
 * To be used with mmap operations only.
 *
 * Parameters:
 * 	* fd : The file number to an open /dev/beaglelogic node
 *
 * Returns:
 * 	0 on success, -1 on failure
 */
int beaglelogic_memcacheinvalidate(int fd);

/* Maps the kernel buffer memory for user inspection / use
 * This gets the size from the beaglelogic device node
 *
 * Parameters:
 * 	* fd : The file number to an open /dev/beaglelogic node
 *
 * Returns:
 * 	NULL if mapping failed, otherwise an address to the buffer
 */
void * beaglelogic_mmap(int fd);

/* Destroys the memory mapping
 * This gets the size from the beaglelogic device node
 *
 * Parameters:
 * 	* fd : The file number to an open /dev/beaglelogic node
 *	* addr : The beginning of the mapping
 *
 * NOTE: Please destroy the previous mapping before setting the buffer
 * size with beaglelogic_set_buffersize. Otherwise strange things will
 * happen!
 *
 * Returns:
 * 	0 if successful
 */
int beaglelogic_munmap(int fd, void *addr);

/* Busy-waits for the next sample buffer to be filled in
 * To be used in conjunction with mmap
 *
 * Parameters:
 * 	none. This uses sysfs attributes, hence no params are required
 *
 * Returns:
 * 	The index of the last buffer read. A negative value indicates the
 * 	state of BeagleLogic when not running (from enum beaglelogic_states)
 */
int beaglelogic_waitfornextbuffer(void);

#endif /* LIBBEAGLELOGIC_H_ */
