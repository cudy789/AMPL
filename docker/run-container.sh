#!/bin/bash

# Corey Knutson, 12/2/2021

# Default to running the noetic image

ARCH=$( uname -m )


# If we're trying to run an arm image on an x86 machine, we need to add an additional flag

IMAGE_NAME="apriltag-multicam"
IMAGE_TAG="latest"

# Start this docker container from the specified image, adding a unique hostname and proper networking
# Also forwards any GUI applications to the host and also adds the current user to the container and mounts its home directory
# We DO NOT want the user's .local folder in the container (it has python libraries, for one), so we make sure to exclude
# it from being mounted by specifying the container path, but no host machine path.

echo "Using image rogueraptor7/$IMAGE_NAME:$IMAGE_TAG"


#  --user=$(id -u $USER):$(id -g $USER) \
#  --volume="/etc/passwd:/etc/passwd:ro" \
#  --volume="/etc/shadow:/etc/shadow:ro" \
#  --volume="$HOME/.local" \

docker run --rm -h $IMAGE_NAME-$HOSTNAME --group-add sudo --group-add video --add-host $IMAGE_NAME-$HOSTNAME:127.0.0.1 --network host -it \
  --user=$(id -u $USER):$(id -g $USER) \
  --volume="/etc/passwd:/etc/passwd:ro" \
  --volume="/etc/shadow:/etc/shadow:ro" \
  --volume="/tmp/.X11-unix:/tmp/.X11-unix:rw" \
  --volume="/var/run/docker.sock:/var/run/docker.sock" \
  --env="DISPLAY" \
  --volume="$HOME:$HOME" \
  --workdir="$(pwd)" \
  --privileged \
  rogueraptor7/$IMAGE_NAME:$IMAGE_TAG /bin/bash
