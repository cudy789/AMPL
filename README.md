# AMPL: Apriltag Multicamera Pose Localization 

[![Documentation Status](https://readthedocs.org/projects/ampl-frc/badge/?version=latest)](https://ampl-frc.readthedocs.io/en/latest/?badge=latest)

## What is AMPL?
Apriltag multicamera pose localization (AMPL) is a position estimator that uses the known location of Apriltag fiducials 
to calculate the exact orientation (or pose) in 3D space. Multiple cameras are used to increase pose accuracy 
and reduce blindspots as the cameras move around in the world. This software stack is geared towards FRC FIRST robotics 
competitions, but can be configured for general Apriltag localization applications.

Compared to existing FRC camera solutions (such as Limelight or PhotonVision), AMPL is designed specifically for multicamera
setups and aims to minimize the technical barrier of entry for multicamera pose localization.

## Hardware

| CoProcessor          | Num Cameras   | FPS |
|----------------------|---------------|-----|
| Raspberry Pi 4 - 4GB | 3 @ 544 x 288 | 30  |
|                      |               |     |


## Setup instructions

View the latest documentation and instructions here.

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
