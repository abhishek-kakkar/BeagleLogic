============
Installation
============

.. note:: The instructions in this document do not apply if you are running the
          :doc:`beaglelogic_system_image`. You can directly proceed to
          :doc:`first_capture`

BeagleLogic has a simple installation script that can be used to install it
on a clean BeagleBone Debian Image.

.. note:: Only `BeagleBoard.org provided Debian images`_ are natively
          supported. If you are using a customized system image build using
          buildroot or OpenEmbedded, additional steps will be required to complete
          installation.

.. _`BeagleBoard.org provided Debian images`: http://elinux.org/Beagleboard:BeagleBoneBlack_Debian


Use the following commands::

    cd <directory-to-install-beaglelogic-in>
    git clone https://github.com/abhishek-kakkar/BeagleLogic
    cd BeagleLogic
    sudo ./install.sh

It is recommended to install BeagleLogic in /opt/BeagleLogic but because it is
not accessible by the default user, some extra steps are needed::

    cd /opt
    sudo git clone https://github.com/abhishek-kakkar/BeagleLogic
    sudo chown -R debian:debian /opt/BeagleLogic
    cd BeagleLogic
    sudo ./install.sh

Replace 'debian' with your own username in case you are not using the default
user on the system.

Upgrading an existing installation
---------------------------------

Do the following::

    cd <path-to-installed-BeagleLogic>
    git pull
    sudo ./install.sh --upgrade

Advanced
--------

You can read the `install.sh`_ file and the `scripts`_ directory for details on how
BeagleLogic is installed, or if you want to manually tweak or customize any
aspect of the installation.

.. _install.sh: https://github.com/abhishek-kakkar/BeagleLogic/blob/master/install.sh
.. _scripts: https://github.com/abhishek-kakkar/BeagleLogic/tree/master/scripts
