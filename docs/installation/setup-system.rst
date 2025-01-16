Setup your system
###################

In this section, we will go through setting up a Raspberry Pi 4 to be used with MAPLE. If you're able to setup a Raspberry Pi Bookworm 64-bit Lite OS
(or similar Linux OS) on your system and configure SSH & internet connectivity on your own, you can skip to :doc:`install`.

**You will need internet on your system when you boot it up, so either configure WiFi or plugin an ethernet cable.**


Imaging your Raspberry Pi
===========================

1. Download the latest version of the `Raspberry Pi Imager Tool <https://www.raspberrypi.com/software/>`_.
2. Plug your microSD card into your computer and open the RPi Imager tool.

   .. image:: /res/installation/rpi-imager.png

3. Select your Raspberry Pi device, the Raspberry Pi OS Lite (64-bit) (under "Choose OS" -> "Raspberry Pi OS, Other"),
   then your storage device (your microSD card).

   .. image:: /res/installation/rpi-imager-select-os.gif

4. Click "Next", then select "Edit Settings".

   .. image:: /res/installation/rpi-imager-edit-settings.png

   Set the hostname to ``maple``, user to ``pi``, and choose a password for the user. Choose the wireless LAN (WiFI) that you
   will connect to and set the password. Make sure to choose the proper country as well. In the locale settings, select your
   timezone.

   .. image:: /res/installation/rpi-imager-general-options.png

5. Click the "Services" tab at the top, then enable SSH by clicking the checkbox.

   .. image:: /res/installation/rpi-imager-ssh.png

6. Click "Save" at the bottom of the customization menu.

   .. image:: /res/installation/rpi-imager-save.png

7. Click "Yes" to apply OS Customisation settings.

   .. image:: /res/installation/rpi-imager-yes.png

8. The imager tool will ask you to make sure you really want to image this storage device, ensure you've selected the correct
   storage, then click "Yes".

   .. image:: /res/installation/rpi-imager-confirm-write.png

9. Wait for the imager to finish writing to the SD card. You should see a confirmation screen once the card has been successfully
   imaged.

   .. image:: /res/installation/rpi-imager-writing.png

   .. image:: /res/installation/rpi-imager-done-writing.png

10. You can unplug your microSD card from your computer now and plug it into the Raspberry Pi.


Connecting to your Raspberry Pi
=================================
This section is assuming you are trying to connect to your Raspberry Pi using a Windows 7/8/10/11 computer.

1. We will connect to our Raspberry Pi over the network using a program called PuTTY. `Download and install PuTTY <https://www.chiark.greenend.org.uk/~sgtatham/putty/latest.html>`_

   .. image:: /res/installation/download-putty.png

2. Make sure your microSD card is plugged into your Raspberry Pi, then plugin the power cable. Your Pi should automatically
   turn on. For Raspberry Pi 5's, you will need to press the power button to turn it on.

   .. note::
      The first time your Pi boots up, it can take more than 2 minutes to fully start. After the first boot, the Pi will usually takes 30 seconds - 1 minute
      to start up.

3. Open up PuTTY on your computer.

   .. image:: /res/installation/putty.png

4. To connect to your Raspberry Pi over the network, type in the IP address or hostname of the Pi. We set this earlier
   to be ``maple.local``. Make sure the "Connection type" is set to "SSH", then click "Open".

   .. note::
      Your computer and Raspberry Pi must be connected to the same network in order to use SSH.

   .. image:: /res/installation/putty-hostname.png

5. The first time you connect to your Pi over PuTTY, you will get the following security alert. Click "Yes" to continue
   connecting.

   .. image:: /res/installation/putty-security-alert.png

6. Type in your username (``pi``), press enter, type in the password you set, then press enter again.

   .. note::
      The letters will not show up as you type them in, this is to keep  your password a secret from people looking at your screen.

   Now you should see the following screen. This is a "terminal" connected to your Raspberry Pi. We will be using the terminal
   in the next steps to install and configure MAPLE.

   .. image:: /res/installation/putty-logged-in.png



.. toctree::
   :maxdepth: 1
