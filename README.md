# MAPLE: Multicamera Apriltag Pose Localization and Estimation

[![Documentation Status](https://readthedocs.org/projects/maple-maple/badge/?version=latest)](https://maple-maple.readthedocs.io/en/latest/?badge=latest)
![GitHub Actions Workflow Status](https://img.shields.io/github/actions/workflow/status/cudy789/maple/build-image.yml?branch=main&label=docker%20image)

![GitHub Actions Workflow Status](https://img.shields.io/github/actions/workflow/status/cudy789/maple/integration-test.yml?branch=main&label=integration%20tests)
![GitHub Actions Workflow Status](https://img.shields.io/github/actions/workflow/status/cudy789/maple/unit-test.yml?branch=main&label=unit%20tests)


## What is MAPLE?

![](./docs/res/crappy_recording_athome.gif)

Multicamera Apriltag Pose Localization and Estimation (MAPLE) is a position estimator that uses the known location of 
[Apriltag](https://github.com/AprilRobotics/apriltag) fiducials 
to estimate the orientation (or pose) of a robot in 3D space. Multiple cameras are used to increase pose accuracy 
and reduce blindspots as the cameras move around in the world. This software stack is geared towards FRC FIRST robotics 
competitions, but can be configured for general Apriltag localization applications.

Compared to existing FRC camera solutions (such as [Limelight](https://docs.limelightvision.io/) or [PhotonVision](https://docs.photonvision.org)), MAPLE is designed specifically for multicamera
setups and aims to minimize the technical barrier of entry for multicamera pose localization.

Bring your own hardware, or use the recommended specs listed below (3 cameras, RPi 5, totalling <$300)

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

## Recommended Hardware

| CoProcessor          | Cameras                                                            | FPS |
|----------------------|--------------------------------------------------------------------|-----|
| Raspberry Pi 5 - 8GB | (3x) Arducam OV 9281 640p 100FPS global shutter camera @ 640 x 480 | 60  |
|                      |                                                                    |     |

## Installation

[View the latest documentation and instructions here.](https://maple-maple.readthedocs.io)
