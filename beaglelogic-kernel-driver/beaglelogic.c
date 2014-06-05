/*
 *
 * Kernel module for BeagleLogic
 *
 * Copyright (C) 2014 Kumar Abhishek <abhishek@theembeddedkitchen.net>
 *
 * This file and the resultant kernel module are licensed under the terms
 * of the GNU General Public license. The program is provided as-is
 * without any implied liabilities or warranties.
 *
 */

#include <asm/atomic.h>
#include <asm/uaccess.h>

#include <linux/module.h>
#include <linux/kernel.h> 
#include <linux/init.h>

#include <linux/platform_device.h>

#include <linux/dma-mapping.h>

#include <linux/kobject.h>
#include <linux/string.h>

#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/of_address.h>
#include <linux/of_device.h>

#include <linux/sysfs.h>
#include <linux/fs.h>

/* Try memory allocation whenever the file is opened */
static ssize_t beaglelogic_show(struct device *dev,
        struct device_attribute *attr, char *buf) {
	int sz = 0, i = 0;
	void *ptr[16] = { 0 }, *pptr[16] = { 0 };

	sz = scnprintf(buf, PAGE_SIZE, "Allocating dma memory blocks...\n");

	for (i = 0; i < 16; i++) {
		pptr[i] = dma_alloc_coherent(dev, 8388608, &ptr[i],
		        GFP_KERNEL | GFP_ATOMIC);

		if (pptr != NULL) {
			sz += scnprintf(buf + sz, PAGE_SIZE,
			        "phyaddr=%08X virtaddr=%08X..\n", ptr[i], pptr[i]);
		} else {
			sz += scnprintf(buf + sz, PAGE_SIZE,
			        "Error in allocating chunk %d..\n", i + 1);
			break;
		}
	}

	for (i = 0; i < 16; i++) {
		if (pptr[i])
			dma_free_coherent(dev, 8388608, pptr[i], ptr[i]);
	}

	sz += scnprintf(buf + sz, PAGE_SIZE, "Cleaned up\n");

	return sz;
}

static ssize_t beaglelogic_store(struct device *dev,
        struct device_attribute *attr, const char *buf, size_t count) {
	printk(KERN_INFO "sysfs store called\n");
	return scnprintf(data, sizeof(data), "%s", buf);
}

static DEVICE_ATTR(beaglelogic_sysfs, S_IWUSR | S_IRUGO, beaglelogic_show, beaglelogic_store);

static int beaglelogic_probe(struct platform_device *pdev) {
	int err;
	struct device *dev = &pdev->dev;
	printk(KERN_INFO "BeagleLogic Initializing...\n");

	err = device_create_file(dev, &dev_attr_beaglelogic_sysfs);
	if (err != 0) {
		printk(KERN_INFO "sysfs file creation failed");
	}

	return 0;
}

static int beaglelogic_remove(struct platform_device *pdev) {
	struct device *dev = &pdev->dev;
	printk(KERN_INFO "BeagleLogic unloading... \n");

	device_remove_file(dev, &dev_attr_beaglelogic_sysfs);

	return 0;
}

static const struct of_device_id beaglelogic_dt_ids[] = {
	{ .compatible = "ti,beaglelogic", .data = NULL, },
	{ /* sentinel */ },
};

static const struct of_device_id pruss_dt_ids[] = {
	{ .compatible = "ti,pruss-v1", .data = NULL, },
	{ .compatible = "ti,pruss-v2", .data = NULL, },
	{ /* sentinel */ },
};

MODULE_DEVICE_TABLE(of, pruss_dt_ids);

static struct platform_driver beaglelogic_driver = {
	.driver = {
		.name = "BeagleLogic",
		.owner = THIS_MODULE,
		.of_match_table = beaglelogic_dt_ids,
	},
	.probe = beaglelogic_probe,
	.remove = beaglelogic_remove,
};

static int __init beaglelogic_init(void) {
	printk(KERN_INFO "BeagleLogic Loaded\n");
	platform_driver_register(&beaglelogic_driver);

	return 0;
}

static void __exit beaglelogic_exit(void) {
	printk(KERN_INFO "BeagleLogic driver unloaded\n");
	platform_driver_unregister(&beaglelogic_driver);
}

module_init(beaglelogic_init);
module_exit(beaglelogic_exit);

MODULE_DESCRIPTION("Kernel Driver for BeagleLogic - a logic analyzer for the BeagleBone [Black]");
MODULE_AUTHOR("Kumar Abhishek <abhishek@theembeddedkitchen.net>");
MODULE_LICENSE("GPL");


