#!/bin/bash

log='beaglelogic-startup:'
SLOTS="/sys/devices/platform/bone_capemgr/slots"
board=$(cat /proc/device-tree/model | sed "s/ /_/g" | tr -d '\000')
is_standalone="N"

if [ "x${board}" = "xTI_AM335x_BeagleLogic_Standalone" ] ; then
	is_standalone="Y"

	# Enable OE of 74LVCH16T245 buffer by writing a '1' to GPIO58
	if [ ! -d "/sys/class/gpio/gpio58" ] ; then
		echo 58 > /sys/class/gpio/export
	fi
	echo out > /sys/class/gpio/gpio58/direction
	echo 1 > /sys/class/gpio/gpio58/value

	# Switch logic level voltage threshold to 3.3V by writing a '1' to GPIO57
	# Writing '0' switches the threshold to 1.8V
	if [ ! -d "/sys/class/gpio/gpio57" ] ; then
		echo 57 > /sys/class/gpio/export
	fi
	echo out > /sys/class/gpio/gpio57/direction
	echo 1 > /sys/class/gpio/gpio57/value

	# Ethernet interrupt pacing to improve performance
	ethtool -C eth0 rx-usecs 500
fi

check_uboot_overlay=$(grep bone_capemgr.uboot_capemgr_enabled=1 /proc/cmdline || true)
if [ ! -d /sys/devices/virtual/misc/beaglelogic ] ; then
	if [ "x${check_uboot_overlay}" = "x" ] && [ "x${is_standalone}" = "xN" ] ; then
		echo beaglelogic > "${SLOTS}"
		echo cape-universalh > "${SLOTS}"
	fi

	echo "${log} Waiting for BeagleLogic to show up (timeout in 120 seconds)"
	wait_seconds=120
	until test $((wait_seconds--)) -eq 0 -o -d "/sys/devices/virtual/misc/beaglelogic" ; do sleep 1; done
	if [ ! -d /sys/devices/virtual/misc/beaglelogic ] ; then
		echo "${log} timeout. BeagleLogic couldn't load."
		exit 1
	fi
fi

if [ "x${is_standalone}" = "xN" ] ; then
	echo "${log} Configuring LA pins"
	config-pin -f $(dirname $0)/pinconfig
fi

echo "${log} Allocating 128MiB of logic buffer to BeagleLogic"
echo 134217728 > /sys/devices/virtual/misc/beaglelogic/memalloc

echo "${log} Loaded"
