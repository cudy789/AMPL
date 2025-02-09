Contributing
###############


Setup your computer
=====================

Windows
~~~~~~~~~

1. Download and install Windows Subsystem for Linux (WSL). Run the following command in powershell.
   More info here: `https://learn.microsoft.com/en-us/windows/wsl/install <https://learn.microsoft.com/en-us/windows/wsl/install>`_

   .. code-block:: bash

      wsl --install

   Run all following commands in the Ubuntu terminal

2. `Download & install Docker desktop <https://www.docker.com/>`_

3. In Docker desktop, in Settings -> Resources -> WSL Integration, check "Enable integration for WSL"

4. Download the source code by running the following command in your Ubuntu terminal

   .. code-block:: bash

      git clone https://github.com/cudy789/MAPLE.git

5. Navigate to the root directory of the repository, then build & run

   .. code-block:: bash

      cd AMPL
      scripts/build-and-run.sh

   Press ctrl-c to kill the container. If the container hangs forever, in a new terminal, type ``docker kill maple``


Guides
===================

.. toctree::
   :maxdepth: 1

   modifying-docs
   simulation