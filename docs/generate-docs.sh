#!/bin/bash

IMAGE_NAME="apriltag-multicam"
IMAGE_TAG="latest"

if [ "$ARM" = "1" ] || [ "$( uname -m )" = "aarch64" ]; then
  ARCH="--platform=linux/arm64"
  if [ "$( uname -m )" != "aarch64" ]; then
    docker run --rm --privileged multiarch/qemu-user-static --reset -p yes # enable qemu support for docker
  fi
else
  ARCH="--platform=linux/amd64"
  IMAGE_TAG="X64"
fi

echo "Using image rogueraptor7/$IMAGE_NAME:$IMAGE_TAG"

echo "Killing any existing ampl containers"
docker kill ampl-docs
sleep 5

echo "Pulling latest image"
docker pull $ARCH rogueraptor7/$IMAGE_NAME:$IMAGE_TAG

docker run --rm -h $IMAGE_NAME-$HOSTNAME --name ampl-docs --group-add sudo --group-add video --add-host $IMAGE_NAME-$HOSTNAME:127.0.0.1 --network host \
  --user=$(id -u $USER):$(id -g $USER) \
  --volume="/etc/passwd:/etc/passwd:ro" \
  --volume="/etc/shadow:/etc/shadow:ro" \
  --volume="/tmp/.X11-unix:/tmp/.X11-unix:rw" \
  --volume="/var/run/docker.sock:/var/run/docker.sock" \
  --env="DISPLAY" \
  --volume="$HOME:$HOME" \
  --workdir="$(pwd)" \
  --privileged \
  $ARCH \
  rogueraptor7/$IMAGE_NAME:$IMAGE_TAG /bin/bash -c "doxygen Doxyfile; make html"