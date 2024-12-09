.. AMPL documentation master file, created by
   sphinx-quickstart on Wed Nov 13 23:26:55 2024.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

Overview
###########

.. important::
    This project is under active development, use at your own risk!

Apriltag multicamera pose localization (AMPL) is a position estimator that uses the known location of
`Apriltag <https://github.com/AprilRobotics/apriltag>`_ fiducials
to estimate the orientation (or pose) of a robot in 3D space. Multiple cameras are used to increase pose accuracy
and reduce blindspots as the cameras move around in the world. This software stack is geared towards FRC FIRST robotics
competitions, but can be configured for general Apriltag localization applications.

.. image:: res/crappy_recording_athome.gif


Compared to existing FRC camera solutions (such as `Limelight <https://docs.limelightvision.io/>`_ or `PhotonVision <https://docs.photonvision.org>`_), AMPL is designed specifically for multicamera
setups and aims to minimize the technical barrier of entry for multicamera pose localization.

Features
=========

* Realtime multicamera (3+) Apriltag pose localization
* Web UI camera and pose visualization
* Camera distortion correction
* Onboard pose trajectory logging
* Compatable with Limelight `fmap` Apriltag field layout files
* Easy installation with Docker compose
* FRC getting started code examples
* Fully documented API
* Stream data over NetworkTables (FRC) or WebSockets


Installation
==============


1. :doc:`installation/choosing-hardware`
2. :doc:`installation/setup-system`
3. :doc:`installation/install`
4. :doc:`installation/configure`
5. :doc:`installation/run`

:doc:`./updating`

FAQ
====

Detection and Estimation Methods
=================================


.. toctree::
   :maxdepth: 1

   self
   installation/index
   api/index