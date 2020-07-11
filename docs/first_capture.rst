Making your first capture
=========================

Follow the steps below to make your first capture using BeagleLogic.

Assuming you are running the BeagleLogic system image and you have booted up, and
BeagleLogic device is available at '/dev/beaglelogic'

Using dd
--------

Simply do::

    dd if=/dev/beaglelogic of=myfile.bin bs=1M size=20

This will capture 20MB worth logic data (20MSamples if you are in the 8-bit mode
and 10MSamples if you are in 16-bit mode)

To modify the sample rate, and other aspects of the capture, go to :doc:`sysfs_attributes`

Using sigrok
------------

Use::

    sigrok-cli -d beaglelogic:logic_channels=8 -c samplerate=10M --samples 10M -o capture.sr

To switch between 8-bit mode and 16-bit mode, replace 8 with 14
(maximum 14 channels are possible)

The other switches are self-explanatory. This command captures 10MSamples at
10MSamples/second and saves it into a file 'capture.sr'. The capture.sr is a
sigrok-specific file format that can be downloaded and opened in the
PulseView software.
