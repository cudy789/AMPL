# Setup instructions

Git pull

git submodule update --init --recursive

./build-and-run.sh

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
