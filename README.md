BeagleLogic
===========

A Google Summer of Code 2014 Project with BeagleBoard.org.

For detailed information and usage guide refer to
[the project wiki](https://github.com/abhishek-kakkar/BeagleLogic/wiki)

BeagleLogic is a Software Suite that implements a logic analyzer with the PRU 
on the BeagleBone / the BeagleBone Black.

This repository does not include the PRU bindings for libsigrok. See
[here](https://github.com/abhishek-kakkar/libsigrok) for the development work 
of BeagleLogic related to libsigrok. The patches will be upstreamed once the
integration is completed.

The repo includes sources for:
 * PRU Firmware
 * BeagleLogic kernel module
 * Device tree overlay

In due course of time, this repo will include:
 * the Server utilizing libsigrok running on the BeagleBone \(Black\)
 * the BeagleLogic HTML5 frontend
 
Selected binaries and archives related to the project may be downloaded from
[this link](http://goo.gl/770FTZ). Refer to the wiki for more information 
regarding the use of the archives and the binaries.

License
--------

 * **PRU firmware & Device tree overlay** : GPLv2
 * **Kernel Module**: GPLv2+
 * **sigrok bindings**: GPLv3+
 * **Web interface** [/beaglelogic-server and /webapp]: MIT
 
The project as a whole is licensed under GPL v3+

A copy of the GNU GPL v3 can be found at the LICENSE file.

