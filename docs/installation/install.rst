Installing AMPL
###################

Use the installation script to download and install AMPL. By default, the script will attempt to configure your network
interface ``eth0`` with a static IP following the format ``10.TE.AM.15`` using `NetworkManager`. If you do not want to set a static IP during installation,
or your Linux distribution doesn't use NetworkManager, run the script with ``SKIP_STATIC_IP=1``

If you run the script multiple times, your config.yml and docker-compose.yml files will not be overwritten.

.. code-block:: bash

   # Download the install script
   wget -O install-ampl.sh https://raw.githubusercontent.com/cudy789/AMPL/refs/heads/main/install.sh
   # Run installation
   bash install-ampl.sh
   # Optionally skip static IP assigment
   # SKIP_STATIC_IP=1 bash install-ampl.sh

This script will make a directory called ``ampl-config`` in your user's home directory with a ``docker-compose.yml`` and
a ``config.yml`` file.

Advanced
=========
You can override ``STATIC_IP`` and ``INTERFACE`` in the CLI to change the IP address and interface. You can run the
downloaded ``set-ip.sh`` script at any time to change your static IP configuration.

Removing a static IP
====================

If you accidentally set a static IP on the wrong interface or want to remove the static IP for any reason, use the following commands.

1. Find your connection name that you want to reset

   .. code-block:: bash

      sudo nmcli con show

      NAME                UUID                                  TYPE      DEVICE
      Wired connection 1  355f3a45-d689-36fa-9190-f0fe8b042f93  ethernet  eth0
      -SSID-              56ac3123-dc7f-423b-b7e1-5530c6253199  wifi      wlan0
      lo                  3035e750-2c09-403f-afca-6069d8105155  loopback  lo
      docker0             10ca8bdc-12e3-48dd-afe2-67ee966efd6a  bridge    docker0

2. Reset the connection to use DHCP

   .. code-block:: bash

      sudo nmcli con mod "Wired connection 1" ipv4.method auto ipv4.addresses "" ipv4.gateway "" ipv4.dns ""

3. Apply changes

   .. code-block:: bash

      sudo nmcli con up "Wired connection 1"


.. toctree::
   :maxdepth: 1




