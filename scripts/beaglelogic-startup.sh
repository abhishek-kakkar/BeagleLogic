#!/bin/bash

log='beaglelogic-startup:'

# BeagleLogic should have been already loaded at this point
check_uboot_overlay=$(grep bone_capemgr.uboot_capemgr_enabled=1 /proc/cmdline || true)
if [ ! -d /sys/devices/virtual/misc/beaglelogic ] ; then
	if [ ! 'x${check_uboot_overlay}' = 'x' ] ; then
		echo "${log} BeagleLogic couldn't load via u-boot, check your uEnv.txt"
		exit 1
	else
		config-pin overlay beaglelogic
		config-pin overlay cape-universalh
	fi
fi

echo "${log} Configuring LA pins"
config-pin -f $(dirname $0)/pinconfig

echo "${log} Allocating 64MiB of logic buffer to BeagleLogic"
echo 67108864 > /sys/devices/virtual/misc/beaglelogic/memalloc
