.. BeagleLogic documentation master file, created by
   sphinx-quickstart on Thu Jul  6 17:19:37 2017.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

Welcome to BeagleLogic!
=======================================

.. image:: ../logo.png

Thank you for choosing BeagleLogic! BeagleLogic is a 100Msps logic analyzer
that runs on the BeagleBone including BeagleBone Black, Green and Green Wireless.

The core of the logic analyzer is the 'beaglelogic' kernel module that reserves
memory for and drives the two Programmable Real-Time Units (PRU) via the
remoteproc interface wherein the PRU directly writes logic samples to the
System Memory (DDR RAM) at the configured sample rate one-shot or continuously
without intervention from the ARM core.

BeagleLogic can be used stand-alone for doing binary captures
without any special client software like this::

    dd if=/dev/beaglelogic of=mydump bs=1M count=1

The above commands grab a binary dump at the specified samplerate using the
sysfs attributes.

When used in conjunction with the `sigrok <http://www.sigrok.org/>`_ library,
BeagleLogic supports software triggers and decoding over 70 different digital
protocols. Since the BeagleLogic bindings for libsigrok have been merged
upstream, the latest built-from-source sigrok libraries and tools support
capturing from BeagleLogic.

BeagleLogic also offers a web interface `(demo) <http://beaglelogic.github.io/webapp>`_
which, once installed on the BeagleBone, can be accessed from port 4000 and can
be used for low-volume captures (upto 3K samples). It makes BeagleLogic a useful
tool for beginners as a learning tool about digital protocols.


.. toctree::
   :maxdepth: 2
   :caption: Contents

   beaglelogic_system_image
   install
   differences_between_4_9_and_3_8
   first_capture
