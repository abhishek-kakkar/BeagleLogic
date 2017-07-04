#!/bin/bash

log='beaglelogic-startup:'

check_uboot_overlay=$(grep bone_capemgr.uboot_capemgr_enabled=1 /proc/cmdline || true)
if [ ! -d /sys/devices/virtual/misc/beaglelogic ] ; then
	if [ 'x${check_uboot_overlay}' = 'x' ] ; then
		config-pin overlay beaglelogic
		config-pin overlay cape-universalh
	else
		echo "${log} Waiting for BeagleLogic to show up (timeout in 120 seconds)"
		wait_seconds=120
		until test $((wait_seconds--)) -eq 0 -o -d "/sys/devices/virtual/misc/beaglelogic" ; do sleep 1; done
		if [ ! -d /sys/devices/virtual/misc/beaglelogic ] ; then
			echo "${log} timeout. BeagleLogic couldn't load via u-boot, please check your uEnv.txt"
			exit 1
		fi
	fi
fi

echo "${log} Configuring LA pins"
config-pin -f $(dirname $0)/pinconfig

echo "${log} Allocating 64MiB of logic buffer to BeagleLogic"
echo 67108864 > /sys/devices/virtual/misc/beaglelogic/memalloc

echo "${log} Loaded"
