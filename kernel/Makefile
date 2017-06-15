# Makefile for kernel modules and device tree script
# This file is a part of the BeagleLogic project

# Path to kernel sources relative to current directory
KSRC ?= /lib/modules/$(shell uname -r)/build

# Module targets (run from host)
obj-m := beaglelogic.o

all:
	@make -C $(KSRC) M=$(PWD) ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- modules

clean:
	@make -C $(KSRC) M=$(PWD) ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- clean

# Compile and install device tree
overlay: beaglelogic-00A0.dtbo

beaglelogic-00A0.dtbo: beaglelogic-00A0.dts
	dtc -O dtb -o beaglelogic-00A0.dtbo -b 0 -@ beaglelogic-00A0.dts

deploy_overlay:
	cp -v beaglelogic-00A0.dtbo /lib/firmware
