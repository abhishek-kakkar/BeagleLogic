BeagleLogic System Image
========================

The BeagleLogic system image is the official way to run BeagleLogic on the
BeagleBone hardware.

.. |br| raw:: html

   <br />

Download the image `here <https://goo.gl/RiXGBs>`_ |br|
[Released on 2017-07-13, sha256sum = *be67e3b8a21c054cd6dcae7c50e9e518492d5d1ddaa83619878afeffe59c99bd* ]

Use username:"debian" and password:"temppwd" (without quotes) to log into the image.

**Supported Platforms:** (tested) BeagleBone Black, Seeed Studio BeagleBone Green and
Seeed Studio BeagleBone Green Wireless and (currently untested) BeagleBone Black Wireless and
SanCloud BeagleBone Enhanced.

Instructions
------------

  * Use Etcher_ to flash the downloaded image on an SD card.
  * Insert the SD Card into the BeagleBone
  * Hold down the USER button (usually in the bottom right corner) before
    applying power to the BeagleBone. This is required so that the BeagleBone
    boots from the bootloader in the SD Card.
  * Verify BeagleLogic is running by doing ``ls -l /dev/beaglelogic``. If BeagleLogic
    did not appear at ``/dev/beaglelogic`` proceed to Troubleshooting_.
  * Once booted, you can then continue to :doc:`make your first capture </first_capture>` using BeagleLogic

.. _Etcher: https://etcher.io

Troubleshooting
---------------

  * Execute on a shell ``journalctl | grep beaglelogic``
  * For BeagleLogic loading correctly, the expected output should be something like this::

        Jul 20 04:56:22 beaglebone bash[832]: beaglelogic-startup: Waiting for BeagleLogic to show up (timeout in 120 seconds)
        Jul 20 04:56:45 beaglebone beaglelogic[862]: Express server listening on port 4000
        Jul 20 04:57:05 beaglebone kernel: remoteproc remoteproc1: Booting fw image beaglelogic-pru0-fw, size 62576
        Jul 20 04:57:05 beaglebone kernel: remoteproc remoteproc2: Booting fw image beaglelogic-pru1-fw, size 31996
        Jul 20 04:57:05 beaglebone kernel: misc beaglelogic: Valid PRU capture context structure found at offset 0000
        Jul 20 04:57:05 beaglebone kernel: misc beaglelogic: BeagleLogic PRU Firmware version: 0.3
        Jul 20 04:57:05 beaglebone kernel: misc beaglelogic: Device supports max 128 vector transfers
        Jul 20 04:57:05 beaglebone kernel: misc beaglelogic: Default sample rate=50000000 Hz, sampleunit=1, triggerflags=0. Buffer in units of 4194304 bytes each
        Jul 20 04:57:05 beaglebone bash[832]: beaglelogic-startup: Configuring LA pins
        Jul 20 04:57:06 beaglebone bash[832]: beaglelogic-startup: Allocating 64MiB of logic buffer to BeagleLogic
        Jul 20 04:57:06 beaglebone kernel: misc beaglelogic: Successfully allocated 67108864 bytes of memory.
        Jul 20 04:57:06 beaglebone bash[832]: beaglelogic-startup: Loaded

  * If you see ``beaglelogic-startup: timeout. BeagleLogic couldn't load``, do ``cat /proc/cmdline``
  * If in cmdline you see ``bone_capemgr.uboot_capemgr_enabled=1`` and BeagleLogic still did not load, please `open an issue`_.
  * If in cmdline you do not see ``uboot_capemgr_enabled``, then try::

        sudo -i
        echo beaglelogic > /sys/devices/platform/bone_capemgr/slots
        echo cape-universalh > /sys/devices/platform/bone_capemgr/slots

  * This should cause BeagleLogic to show up in the journalctl output as above.
  * Then do ``sudo service beaglelogic-startup restart`` to initialize BeagleLogic
  * If these steps do not work for you, please `open an issue`_

.. _open an issue: https://github.com/abhishek-kakkar/BeagleLogic/issues


Release Notes
-------------

This image release represents a major leap for BeagleLogic in general.

  * This release is based on Debian 9 [Stretch], which is the latest available stable
    Debian version.
  * The kernel has been upgraded from 3.8.13-bone kernel to the 4.9.36-ti-r46
    release. BeagleLogic has been migrated to run on the 4.9 kernels, this needed
    a firmware and kernel module change. You can read more about it :doc:`here </differences_between_4_9_and_3_8>`
  * Root login is disabled by default for a more secure image. However root permission
    is now no longer required for editing the BeagleLogic sysfs attributes.
  * The underlying sigrok components have been updated to their latest release on
    the `13th of June 2017 <https://http://sigrok.org/blog/major-sigrok-releases-libsigrok-libsigrokdecode-fx2lafw-sigrok-cli-pulseview>`_.
    This brings in support for new protocol decoders and faster protocol decoding.
  * NodeJS version has been upgraded to v6.11 . This is used by the web interface.
  * In order to load BeagleLogic, a overlay needs to be applied to the device tree
    that is used to boot the BeagleBone device. This is now by default applied at boot time
    using U-Boot's built in cape manager support. Hence it is essential that the new
    version of the bootloader bundled in the SD card is used to boot the device. Which means
    it is important to hold down the USER button on the BeagleBone while booting the device.
