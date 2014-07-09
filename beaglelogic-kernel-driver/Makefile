# Makefile for kernel modules and device tree script
# This file is a part of the BeagleLogic project

# Path to kernel sources relative to current directory
KSRC = /lib/modules/$(shell uname -r)/build

# Host name of BBB (like root@192.168.7.2)
BBHOST = root@192.168.7.2

# Module targets (run from host)
obj-m := beaglelogic.o

all:
	@make -C $(KSRC) M=$(PWD) ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- modules

clean:
	@make -C $(KSRC) M=$(PWD) ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- clean

# Deployment targets to be run only from BeagleBone environment as root!	

# Compile and install device tree
devicetree: devicetree-frombb

devicetree-tobb:
	dtc -O dtb -o BB-BEAGLELOGIC-00A0.dtbo -b 0 -@ BB-BEAGLELOGIC-00A0.dts
	scp BB-BEAGLELOGIC-00A0.dtbo $(BBHOST):/lib/firmware

devicetree-frombb:
	dtc -O dtb -o /lib/firmware/BB-BEAGLELOGIC-00A0.dtbo -b 0 -@ BB-BEAGLELOGIC-00A0.dts

# Install the PRU firmware
firmware:
	@make -C $(PWD)/../beaglelogic-firmware install-frombb

firmware-tobb:
	@make -C $(PWD)/../beaglelogic-firmware BBHOST=$(BBHOST) install-tobb
