sysfs attributes Reference
==========================

BeagleLogic can be configured via its sysfs attributes. The attributes can be
found in ``/sys/devices/virtual/misc/beaglelogic``

To write to any sysfs attribute, do::

    echo [value] > /sys/devices/virtual/misc/beaglelogic/[attribute]

To read any sysfs attribute, do::

    cat /sys/devices/virtual/misc/beaglelogic/[attribute]


memalloc
--------

Reserves system memory for BeagleLogic. Upto 300MB or 320MB can be reserved
for data captures.

For example, to reserve 128MiB for data captures for BeagleLogic, do::

    echo 134217728 > /sys/devices/virtual/misc/beaglelogic/memalloc

(1 MiB = 1024 * 1024 bytes)

Buffers are allocated in units of 4 MiB by default. 128MiB means that there will
be 32 such buffers that are filled up in sequence.

.. note:: By default, 64MiB of the system memory (128MiB on the BeagleLogic
          Standalone) is reserved for data captures.

triggerflags
------------

Set this to zero '0' for one-shot logic capture (i.e. fill up the buffer once
and stop), and '1' for continuously capturing data.

triggerflags is set to zero by default. Set it to 1 for continuous captures.

sampleunit
----------

When set to zero '0', BeagleLogic will capture 16-bit samples. When set to '1',
BeagleLogic will only capture 8-bit samples, the first 8 channels [0-7 on the
BeagleLogic Standalone, P8_39 to P8_46 on the BeagleBones].

samplerate
----------

Read or write the sample rate, in Hz. Write any value from 10 to 100000000 Hz (100 MHz).
BeagleLogic supports sample rates of (100 / n) MHz where n is a positive integer.

.. note:: If you are using sample rates < 1 MHz, then you should configure bufunitsize
          accordingly so that the application does not hang for a long time waiting
          for data as the output can be read only in multiple of "bufunitsize" bytes
          For example, at 100kHz sample rate, you will have to wait 40 seconds at the
          default settings before data appears, unless you set bufunitsize to a value
          lower than 4 MiB.

bufunitsize
-----------

Set the unit size for a logic buffer, in bytes. Default at initialization is 4194304 bytes (4 MiB)
Set this to a lower value if using a sample rate less than 4MHz.
