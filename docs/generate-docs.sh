#!/bin/bash

IMAGE_NAME="maple"
IMAGE_TAG="X64"

echo "Using image rogueraptor7/$IMAGE_NAME:$IMAGE_TAG"

echo "Killing any existing maple containers"
docker kill maple-docs
sleep 5

docker run --rm -h $IMAGE_NAME-$HOSTNAME --name maple-docs --group-add sudo --add-host $IMAGE_NAME-$HOSTNAME:127.0.0.1 --network host \
  --user=$(id -u $USER):$(id -g $USER) \
  --volume="/etc/passwd:/etc/passwd:ro" \
  --volume="/etc/shadow:/etc/shadow:ro" \
  --volume="/tmp/.X11-unix:/tmp/.X11-unix:rw" \
  --volume="/var/run/docker.sock:/var/run/docker.sock" \
  --env="DISPLAY" \
  --volume="$HOME:$HOME" \
  --workdir="$(pwd)" \
  --privileged \
  rogueraptor7/$IMAGE_NAME:$IMAGE_TAG /bin/bash -c "doxygen Doxyfile; make html"