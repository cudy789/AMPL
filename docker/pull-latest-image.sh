#!/bin/bash

IMAGE_NAME="maple"
IMAGE_TAG="latest"

if [ "$X64" = "1" ] || [ "$( uname -m )" != "aarch64" ]; then
  IMAGE_TAG="X64"
fi

echo "docker pull rogueraptor7/$IMAGE_NAME:$IMAGE_TAG"

docker pull rogueraptor7/$IMAGE_NAME:$IMAGE_TAG