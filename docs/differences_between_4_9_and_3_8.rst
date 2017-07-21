=============================================================
Differences Between BeagleLogic on 4.9 vs 3.8 kernel versions
=============================================================

BeagleLogic has recently been migrated to run on Linux Kernel Version 4.9 from
the earlier kernel version 3.8.13 it used to run on.

For the user, there should be no noticeable difference in the way things function,
but this article describes all the under-the-hood changes that have enabled this
migration.

Changes:

  * No root permissions are needed to modify the sysfs attributes located under
    ``/sys/devices/virtual/misc/beaglelogic``. This is achieved by adding a udev
    rule to change these attributes' group to beaglelogic and adding the default
    user under the beaglelogic group so that one can modify the sysfs attributes
    without requiring root permissions. This will enable applications that need
    BeagleLogic to also not require running as root which also further enhances
    security.

  * The firmware now has a resource table as needed by the remoteproc framework
    in order to load the firmware into the PRUs and configure interrupts
    correctly.

  * The kernel driver utilizes the latest remoteproc framework from TI, which
    requires firmware loading to be handled by the kernel driver itself.

  * rpmsg framework for message passing is not used, rather communication between
    the kernel driver and the PRU happens through a command-response exchange
    through the PRU0 SRAM. The kernel driver receives interrupts from the PRU
    whenever data is ready and it makes the data then available to userspace.

  * PRU firmware for PRU1 is now a symlink to the actual PRU firmware being used.
    This can be useful to change the firmware so that applications like PRUDAQ
    can be enabled.
