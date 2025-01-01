Choosing your coprocessor and cameras
#######################################

Choosing your coprocessor
============================

If you are building a setup from scratch, the **recommended** system requirements are

* Raspberry Pi 5 8GB w/active cooling
* Raspberry Pi OS Bookworm Lite (64-bit)
* 128GB high endurance microSD card

If you have components on hand, the **minimum** system requirements are

* Raspberry Pi 4 4GB (or equivalent)
* Raspberry Pi OS (64-bit) (or equivalent)
* 32GB storage

A high read/write endurance microSD card, adequate cooling (active coolers are preferred), and a
high quality power supply are **highly recommended**.

AMPL was developed to run on low power ARM single board computers but can also run on x86 machines.

Ensure your system has enough IO for all of your cameras.

Choosing your cameras
=======================
There are many factors to consider when choosing your cameras for AMPL. You can mix and match your camera models, resolutions,
framerates, etc., to fit your use case.

If you are building a setup from scratch, **we recommend 3 high framerate global shutter Arducam modules**, such as the
`OV9281 640p 100FPS monochrome global shutter camera. <https://www.arducam.com/product/arducam-100fps-global-shutter-usb-camera-board-1mp-720p-ov9281-uvc-webcam-module-with-low-distortion-m12-lens-without-microphones-for-computer-laptop-android-device-and-raspberry-pi/>`_

If you have cameras on hand, all USB 2.0/3.0 and CSI cameras are supported. See the important factors below when choosing your cameras.

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

* Currently, fisheye lenses are not supported in AMPL, but distortion correction for pinhole cameras is available. Cameras
  with advertised low distortion lenses can be desirable and remove the need to perform any distortion correction in software,
  which will **dramatically increase performance**.

Global vs. rolling shutter

* Global shutter cameras read an image from all of the camera's pixels at once, reducing motion blur and distortion affects
  on objects that are moving quickly. Global shutter cameras are preferred but not necessary.

Power

* Using many high current draw cameras on a single board computer can cause the system to brownout or
  restart. For the Raspberry Pi 4/5, use a >=20watt USB-C power supply. You can also power the Raspberry Pi via the GPIO pins.

Driver compatibility

* Arducam cameras tend to behave better with OpenCV and how Linux handles UVC devices. Arducam devies are preferred.

.. warning::
    Cameras with known issues:

    * **Logitech C270** Exposure and framerate cannot be directly set. The brighter the scene is when AMPL starts up, the higher the framerate the camera will be locked into.

Less important factors
**********************************************

Sensor size

* Larger sensors will have better low light performance, which may be important for some applications.


Example Configurations
=========================

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
