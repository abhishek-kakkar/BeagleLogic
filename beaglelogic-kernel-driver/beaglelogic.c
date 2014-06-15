/*
 * Kernel module for BeagleLogic - a logic analyzer for the BeagleBone [Black]
 * Designed to be used in conjunction with a modified pru_rproc driver
 *
 * Copyright (C) 2014 Kumar Abhishek <abhishek@theembeddedkitchen.net>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 */

#include <asm/atomic.h>
#include <asm/uaccess.h>

#include <linux/module.h>
#include <linux/kernel.h> 
#include <linux/init.h>
#include <linux/wait.h>

#include <linux/platform_device.h>
#include <linux/miscdevice.h>

#include <linux/mm.h>
#include <linux/dma-mapping.h>

#include <linux/kobject.h>
#include <linux/string.h>

#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/of_address.h>
#include <linux/of_device.h>

#include <linux/sysfs.h>
#include <linux/fs.h>



module_init(beaglelogic_init);
module_exit(beaglelogic_exit);

MODULE_DESCRIPTION("Kernel Driver for BeagleLogic");
MODULE_AUTHOR("Kumar Abhishek <abhishek@theembeddedkitchen.net>");
MODULE_LICENSE("GPL");


