# AMPL: Apriltag Multicamera Pose Localization 

[![Documentation Status](https://readthedocs.org/projects/ampl-frc/badge/?version=latest)](https://ampl-frc.readthedocs.io/en/latest/?badge=latest)

## #Nearing completion, check back soon!# 

## What is AMPL?
Apriltag multicamera pose localization (AMPL) is a position estimator that uses the known location of 
[Apriltag](https://github.com/AprilRobotics/apriltag) fiducials 
to calculate the exact orientation (or pose) in 3D space. Multiple cameras are used to increase pose accuracy 
and reduce blindspots as the cameras move around in the world. This software stack is geared towards FRC FIRST robotics 
competitions, but can be configured for general Apriltag localization applications.

Compared to existing FRC camera solutions (such as [Limelight](https://docs.limelightvision.io/) or [PhotonVision](https://docs.photonvision.org)), AMPL is designed specifically for multicamera
setups and aims to minimize the technical barrier of entry for multicamera pose localization.

## Features

* Realtime multicamera (3+) Apriltag pose localization
* Web UI camera and pose visualization
* Camera distortion correction
* Onboard pose trajectory logging
* Compatable with Limelight `fmap` Apriltag field layout files
* Easy installation with Docker compose
* FRC getting started code examples
* Fully documented API
* Stream data over NetworkTables (FRC) or WebSockets

## Hardware

| CoProcessor          | Cameras                                    | FPS |
|----------------------|--------------------------------------------|-----|
| Raspberry Pi 4 - 4GB | 3 Logitech C270 USB 2.0 @ 544 x 288        | 30  |
|                      |                                            |     |

## Setup

[View the latest documentation and instructions here.](https://ampl-frc.readthedocs.io)

## Lens distortion correction
https://docs.opencv.org/4.x/dc/dbb/tutorial_py_calibration.html

# .fmap files

The origin is always at the center of the field with the X axis pointing towards the blue alliance, y pointing down, 
and z pointing into the field.

Create or modify .fmap files using [Limelight Tools Map Builder](https://tools.limelightvision.io/map-builder)

### at14_6.fmap

Two apriltags (tagID 14 & 6) both located at 1.0, 2.0, 0.75 xyz global coords. Origin is at the center of the field.

### at14zeroed.fmap

A single apriltag (tagID 14) located at 0, 0, 0 xyz global coords. Origin is at the center of the field.

### crescendo_2024.fmap

The full 2024 FRC Crescendo field. Origin is at the center of the field.

### RPi video bandwidth throttling

## THE ISSUE:
The cameras are set to autoexposure before the program gets a hold of them. If they're in low light environments, they 
default to a higher exposure, locking it in at a lower FPS. Need to figure out how to properly adjust the exposure in the code.

`v4l2-ctl --device /dev/video0 --all` <- list camera information, including exposure and auto exposure settings
`v4l2-ctl --device /dev/video0 --list-formats-ext` <- list camera resolution & framerates in all video formats


## Latency

~ 300ms of latency end to end. Not great...