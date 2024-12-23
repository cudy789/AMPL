Choosing your coprocessor and cameras
###################################

Choosing your computer
====================

Minimum system requirements:

* Raspberry Pi 4 4GB (or equivalent)
* Raspberry Pi OS (64-bit) (or equivalent)
* 32GB storage

Recommended system requirements:

* Raspberry Pi 5 8GB w/active cooling
* Raspberry Pi OS Bookworm Lite (64-bit)
* 128GB high endurance microSD card

AMPL was developed to run on low power ARM single board computers. We recommend using a Raspberry
Pi 4 or newer with a high read/write endurance microSD card, adequate cooling (active coolers are preferred), and a
high quality power supply.

AMPL can run also run on X86_64 CPUs.

Ensure your system has enough IO for all of your cameras.

Choosing your cameras
=======================
There are many factors to consider when choosing your cameras for AMPL. You can mix and match your camera models, resolutions,
framerates, etc., to fit your use case.

USB 2.0/3.0 and CSI webcams are supported. Ensure your computer has fast enough IO to handle higher speed devices and you don't
exceed the USB 2.0/3.0 controller bandwidth limit.

Important factors
*******************

Resolution

* Apriltags can be detected from further away with higher resolution cameras. However, CPU load significantly increases
  with resolution. Finding the minimum resolution for your desired application is critical to improve performance. Most cameras
  support a variety of resolutions lower than their advertised resolution.

Framerate

* Higher framerate cameras will have less blur during fast movements and will have lower latency during pose estimation.

Number of cameras

* Additional cameras will decrease blindspots, increase accuracy, but also increase CPU load. Find a balance between
  the number of cameras and the desired visibility/accuracy.

Lens

* Currently, fisheye lenses are not supported in AMPL, but distortion correction for pinhole cameras is available.

Power

* Using many high current draw cameras on a single board computer can cause the system to brownout or
  restart.

Driver compatibility

* Arducam cameras tend to behave better with OpenCV and how Linux handles UVC devices.

.. warning::
    Cameras with known issues:

    * **Logitech C270** Exposure and framerate cannot be directly set. The brighter the scene is when AMPL starts up, the higher the framerate the camera will be locked into.

Less important factors
**********************************************

Sensor size

* Larger sensors will have better low light performance, which may be important for some applications


Example Configurations
=======================

.. list-table::
    :widths: 25 25 50
    :header-rows: 1

    * - Co-processor
      - Camera
      - FPS

    * - Raspberry Pi 4 - 4GB
      - 3 Logitech C270 USB 2.0 @ 544 x 288
      - 3 @ 30


.. toctree::
   :maxdepth: 1
