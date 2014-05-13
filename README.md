BeagleLogic
===========

A Google Summer of Code 2014 Project with BeagleBoard.org.

BeagleLogic is a Software Suite that implements a logic analyzer with the PRU 
on the BeagleBone / the BeagleBone Black.

This repository does not include the PRU bindings for libsigrok. See
[here](https://github.com/abhishek-kakkar/libsigrok) for the development work 
of BeagleLogic related to libsigrok. The patches will be upstreamed once the
integration is completed.

In due course of time, this repo will include:
 * the device tree descriptors
 * PRU Firmware sources and binaries
 * the Server utilizing libsigrok running on the BeagleBone \(Black\)
 * the BeagleLogic HTML5 frontend


Setting up the build environment
--------------------------------

Clone this git repository and in the same directory, clone:
 * libserialport
 * libsigrok
 * libsigrokdecode
 * sigrok-cli
 
Also, copy the necessary libraries and pkgconfigs from the system image into 
/usr in this directory. 

A set of minimum libraries taken from the Debian system image may be downloaded
\(link shortly\).

This application also requires libprussdrv to be installed on the BeagleBone
Black. Note that if you are running Debian on your BeagleBone, this has been
already installed, and you need to copy prussdrv.h, prussdrv_intc.h into
\[/home/\<username\>/BeagleLogic\]/usr/include and and libprussdrv.* files into 
\[/home/\<username\>/BeagleLogic\]/usr/lib with the other libraries.

libprussdrv is autodetected by the libsigrok fork, which auto-enables the PRU 
driver in libsigrok while configuration is done. If libprussdrv is not found,
PRU support may not be enabled.

Testing
--------

The software bundled in this repository is tested on the BeagleBone Black
running Debian released on 23rd April 2014 running the 3.8.13-bone47 kernel.

The build environment is Ubuntu 14.04 LTS (Trusty Tahr). The cross compiler
is arm-linux-gnueabihf \[Linaro 4.8\].

License
--------

The project as a whole is licensed under GPLv3+ . The files may have individual
licenses compatible with the GNU GPL, but that does not affect the fact that 
the complete work is under the GPL.

A copy of the GNU GPL v3 can be found at the License file.
