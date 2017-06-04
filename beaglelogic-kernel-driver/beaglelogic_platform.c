/*
 * Platform driver for BeagleLogic
 *
 * Copyright (C) 2017 Kumar Abhishek <abhishek@theembeddedkitchen.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 */

#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/pruss.h>

#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/of_address.h>
#include <linux/of_device.h>

typedef struct buflist {
	uint32_t dma_start_addr;
	uint32_t dma_end_addr;
} bufferlist;

struct capture_context {
	uint32_t magic;

	uint32_t cmd;
	uint32_t resp;

	uint32_t samplediv;
	uint32_t sampleunit;
	uint32_t triggerflags;

	bufferlist list[3];
};

struct bldata {
	int to_bl_irq;
	int from_bl_irq_1;
	int from_bl_irq_2;

	int intcnt;

	struct pruss *pruss;
	struct rproc *pru0;
	struct pruss_mem_region pru0sram;

	struct capture_context *cxt_pru;
};

static irqreturn_t beaglelogic_interrupt_handler(int irq, void *data)
{
	struct device *dev = data;
	struct bldata *bdata = dev_get_drvdata(dev);

	if (irq == bdata->from_bl_irq_1) {
		bdata->intcnt++;
		dev_info(dev, "Got from_bl_1 IRQ# %d, %d\n", irq, bdata->intcnt);
		if (bdata->intcnt == 10) {
			dev_info(dev, "Got 10 interrupts, stopping\n");
			pruss_intc_trigger(bdata->to_bl_irq);
		}
	}
	else if (irq == bdata->from_bl_irq_2) {
		dev_info(dev, "Got from_bl_2 IRQ# %d\n", irq);
	}

	return IRQ_HANDLED;
}

static int beaglelogic_send_cmd(struct bldata *bdata, uint32_t cmd)
{
	uint32_t timeout = 200;

	bdata->cxt_pru->cmd = cmd;

	/* Wait for firmware to process the command */
	while (--timeout && bdata->cxt_pru->cmd != 0)
		cpu_relax();

	if (timeout == 0)
		return -1;

	return bdata->cxt_pru->resp;
}

static int beaglelogic_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct bldata *bdata;
	int ret;
	uint32_t test;

	bdata = kzalloc(sizeof(*bdata), GFP_KERNEL);
	if (!bdata)
		goto fail;

	dev_set_drvdata(dev, bdata);

	bdata->from_bl_irq_1 = platform_get_irq_byname(pdev, "from_bl_1");
	dev_info(dev, "Got from_bl1 IRQ#%d\n", bdata->from_bl_irq_1);
	if (bdata->from_bl_irq_1 <= 0)
		goto faild;

	bdata->from_bl_irq_2 = platform_get_irq_byname(pdev, "from_bl_2");
	dev_info(dev, "Got from_bl2 IRQ#%d\n", bdata->from_bl_irq_2);
	if (bdata->from_bl_irq_2 <= 0)
		goto faild;

	bdata->to_bl_irq = platform_get_irq_byname(pdev, "to_bl");
	dev_info(dev, "Got to_bl IRQ#%d\n", bdata->to_bl_irq);
	if (bdata->to_bl_irq <= 0)
		goto faild;

	ret = request_irq(bdata->from_bl_irq_1, beaglelogic_interrupt_handler,
		IRQF_ONESHOT, dev_name(dev), dev);
	if (ret) goto faild;

	ret = request_irq(bdata->from_bl_irq_2, beaglelogic_interrupt_handler,
		IRQF_ONESHOT, dev_name(dev), dev);
	if (ret) goto faild;

	dev_info(dev, "Loading BeagleLogic driver");

	bdata->pruss = pruss_get(dev);
	if (IS_ERR(bdata->pruss)) {
		ret = PTR_ERR(bdata->pruss);
		if (ret != -EPROBE_DEFER)
			dev_err(dev, "Unable to get pruss handle.\n");
		goto fail_free_irq;
	}

	bdata->pru0 = pruss_rproc_get(bdata->pruss, PRUSS_PRU0);
	if (IS_ERR(bdata->pru0)) {
		ret = PTR_ERR(bdata->pru0);
		if (ret != -EPROBE_DEFER)
			dev_err(dev, "Unable to get PRU0.\n");
		goto pruss_put;
	}

	ret = pruss_request_mem_region(bdata->pruss, PRUSS_MEM_DRAM0,
		&bdata->pru0sram);
	if (ret) {
		dev_err(dev, "Unable to get PRUSS RAM.\n");
		goto putmem;
	}

	test = *((uint32_t*)bdata->pru0sram.va);
	dev_info(dev, "Got value 0x%08X at loc 0000\n", test);

	if (test == 0xBEA61E10)
	{
		bdata->cxt_pru = bdata->pru0sram.va;

		ret = beaglelogic_send_cmd(bdata, 1);
		dev_info(dev, "Resp = %08X for CMD=1\n", ret);

		ret = beaglelogic_send_cmd(bdata, 2);
		dev_info(dev, "Resp = %08X for CMD=2\n", ret);

		bdata->cxt_pru->samplediv = 4000;
		bdata->cxt_pru->sampleunit = 1;
		bdata->cxt_pru->triggerflags = 1;

		bdata->cxt_pru->list[0].dma_start_addr = 0x10000;
		bdata->cxt_pru->list[0].dma_end_addr = 0x10040;
		bdata->cxt_pru->list[1].dma_start_addr = 0x10040;
		bdata->cxt_pru->list[1].dma_end_addr = 0x10080;
		bdata->cxt_pru->list[2].dma_start_addr = 0;
		bdata->cxt_pru->list[2].dma_end_addr = 0;

		bdata->intcnt = 0;

		ret = beaglelogic_send_cmd(bdata, 3);
		dev_info(dev, "Resp = %08X for CMD=3\n", ret);

		ret = beaglelogic_send_cmd(bdata, 4);
		dev_info(dev, "Resp = %08X for CMD=4\n", ret);
	}

	return 0;

putmem:
	if (bdata->pru0sram.va)
		pruss_release_mem_region(bdata->pruss, &bdata->pru0sram);
pru0_put:
	pruss_rproc_put(bdata->pruss, bdata->pru0);
pruss_put:
	pruss_put(bdata->pruss);
fail_free_irq:
	free_irq(bdata->from_bl_irq_1, dev);
	free_irq(bdata->from_bl_irq_2, dev);
faild:
	kfree(bdata);
fail:
	return ret;
}

static int beaglelogic_remove(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct bldata *bdata = dev_get_drvdata(dev);

	dev_info(dev, "%s: removing BeagleLogic driver\n", __func__);

	pruss_release_mem_region(bdata->pruss, &bdata->pru0sram);
	pruss_rproc_put(bdata->pruss, bdata->pru0);
	pruss_put(bdata->pruss);

	free_irq(bdata->from_bl_irq_1, dev);
	free_irq(bdata->from_bl_irq_2, dev);

	kfree(bdata);

	return 0;
}

static const struct of_device_id beaglelogic_dt_ids[] = {
	{ .compatible = "beaglelogic,beaglelogic", .data = NULL, },
	{ /* sentinel */ },
};

static struct platform_driver beaglelogic_driver = {
	.driver = {
		.name = "beaglelogic",
		.owner = THIS_MODULE,
		.of_match_table = beaglelogic_dt_ids,
	},
	.probe = beaglelogic_probe,
	.remove = beaglelogic_remove,
};

module_platform_driver(beaglelogic_driver);

MODULE_AUTHOR("Kumar Abhishek <abhishek@theembeddedkitchen.net>");
MODULE_DESCRIPTION("Platform driver for BeagleLogic");
MODULE_LICENSE("GPL v2");
MODULE_VERSION("1.2");
