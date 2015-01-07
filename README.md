![logo](logo.png)
===========

NEW: The BeagleLogic cape is here! To get more information please click [here]
(https://github.com/abhishek-kakkar/BeagleLogic/wiki/The-BeagleLogic-Cape).

Bootstrapped as a Google Summer of Code 2014 Project with BeagleBoard.org.

For detailed information and usage guide refer to
[the project wiki](https://github.com/abhishek-kakkar/BeagleLogic/wiki)

BeagleLogic realizes a logic analyzer on the BeagleBone / the BeagleBone Black using
the Programmable Real-Time units and matching firmware and Linux kernel modules on the
BeagleBone Black.

The software should also work on the BeagleBone White, but with limited memory support
(only 256 MB instead of 512 MB)

BeagleLogic can also be used in conjunction with the sigrok projects to capture and process
the data on the BeagleBone itself. The libsigrok bindings for BeagleLogic have been accepted
into the upstream libsigrok repository.

Directories:

* beaglelogic-firmware: PRU Firmware
* beaglelogic-kernel-driver: Device Tree overlay source and kernel module source and Makefile.
**The default BeagleBone kernel v3.8.13-bone60 and above ship with BeagleLogic support.
You can verify it using the command** ```modinfo beaglelogic```
* beaglelogic-server: Node.JS backend and static file server for the web interface
* cape: The cape design files (KiCAD schematic and PCB file only. Production Gerbers are available separately.)
* webapp: A minimal web client for BeagleLogic. Uses sigrok-cli internally for data
acquisition
* testapp: A simple test application that shows how to use the userspace API of BeagleLogic
and benchmarks memory copy speeds.
 
Selected binaries and archives related to the project may be downloaded from
[this link](http://goo.gl/770FTZ). Refer to the wiki for more information 
regarding the use of the archives and the binaries.

License
--------

 * **PRU firmware & Device tree overlay** : GPLv2
 * **Kernel Module**: GPLv2+
 * **sigrok bindings**: GPLv3+
 * **Web interface** [/beaglelogic-server and /webapp]: MIT
 * **Cape** : CERN Open Hardware License
 * **Logo** : [Creative Commons Attribution-ShareAlike 4.0 International (CC-BY-SA-4.0)]
(http://creativecommons.org/licenses/by-sa/4.0/)

