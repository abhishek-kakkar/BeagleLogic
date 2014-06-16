/*
 * Userspace/Kernelspace common API for BeagleLogic
 * ioctl commands and enumeration of states
 *
 * Copyright (C) 2014 Kumar Abhishek <abhishek@theembeddedkitchen.net>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 */

#ifndef BEAGLELOGIC_H_
#define BEAGLELOGIC_H_

enum {
	STATE_BL_DISABLED,	/* Powered off (at module start) */
	STATE_BL_INITIALIZED,	/* Powered on */
	STATE_BL_MEMALLOCD,	/* Buffers allocated */
	STATE_BL_ARMED,		/* All Buffers DMA-mapped and configuration done */
	STATE_BL_RUNNING,	/* Data being captured */
	STATE_BL_REQUEST_STOP,	/* Stop requested */
	STATE_BL_ERROR   	/* Buffer overrun */
};

/* ioctl calls that can be issued on /dev/beaglelogic */

#define IOCTL_BL_GET_VERSION        _IOR('k', 0x20, u32)

#define IOCTL_BL_GET_SAMPLE_RATE    _IOR('k', 0x21, u32)
#define IOCTL_BL_SET_SAMPLE_RATE    _IOW('k', 0x21, u32)

#define IOCTL_BL_GET_SAMPLE_UNIT    _IOR('k', 0x22, u32)
#define IOCTL_BL_SET_SAMPLE_UNIT    _IOW('k', 0x22, u32)

#define IOCTL_BL_GET_TRIGGER_FLAGS  _IOR('k', 0x23, u32)
#define IOCTL_BL_SET_TRIGGER_FLAGS  _IOW('k', 0x23, u32)

#define IOCTL_BL_GET_CUR_INDEX      _IOR('k', 0x24, u32)
#define IOCTL_BL_CACHE_INVALIDATE    _IO('k', 0x25)

#define IOCTL_BL_FILL_TEST_PATTERN   _IO('k', 0x26)

#define IOCTL_BL_START               _IO('k', 0x27)
#define IOCTL_BL_STOP                _IO('k', 0x28)

#endif /* BEAGLELOGIC_H_ */
