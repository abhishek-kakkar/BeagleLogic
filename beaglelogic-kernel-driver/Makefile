# Makefile for kernel modules and device tree script
# This file is a part of the BeagleLogic project

# Path to kernel sources relative to current directory
KSRC = $(PWD)/../../BeagleBone/kernel/kernel

# Module targets (run from host)
obj-m := beaglelogic.o pru_rproc_bl.o

all:
	@make -C $(KSRC) M=$(PWD) ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- modules

clean:
	@make -C $(KSRC) M=$(PWD) ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- clean

# Deployment targets to be run only from BeagleBone environment as root!	

# Compile and install device tree
devicetree:
	dtc -O dtb -o /lib/firmware/bb-beaglelogic-00A0.dtbo -b 0 -@ bb-beaglelogic.dts

# Install the PRU firmware
firmware:
	@make -C $(PWD)/../bl-firmware install

