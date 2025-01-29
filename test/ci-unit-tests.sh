#!/bin/bash

# Corey Knutson, 12/2/2021

GTESTFILTER=""
if [ -n "$1" ]; then
    GTESTFILTER="--gtest_filter=$1"
fi

IMAGE_NAME="maple"
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

# Start this docker container from the specified image, adding a unique hostname and proper networking
# Also forwards any GUI applications to the host and also adds the current user to the container and mounts its home directory

echo "Using image rogueraptor7/$IMAGE_NAME:$IMAGE_TAG"

echo "Pulling latest image"
docker pull $ARCH rogueraptor7/$IMAGE_NAME:$IMAGE_TAG

echo "Killing any existing maple containers"
docker kill maple-tests
sleep 5

echo "./runTests $GTESTFILTER"

docker run --rm -h $IMAGE_NAME-$HOSTNAME --name maple-tests --group-add sudo --group-add video --add-host $IMAGE_NAME-$HOSTNAME:127.0.0.1 --network host \
  --user=$(id -u $USER):$(id -g $USER) \
  --volume="/etc/passwd:/etc/passwd:ro" \
  --volume="/etc/shadow:/etc/shadow:ro" \
  --volume="/tmp/.X11-unix:/tmp/.X11-unix:rw" \
  --volume="/var/run/docker.sock:/var/run/docker.sock" \
  --env="DISPLAY" \
  --volume="$(pwd):$(pwd)" \
  --workdir="$(pwd)" \
  --privileged \
  $ARCH \
  rogueraptor7/$IMAGE_NAME:$IMAGE_TAG /bin/bash -c "mkdir -p build && cd build && cmake -DENABLE_TESTS=ON .. && make -j4 && ./runTests $GTESTFILTER;"
