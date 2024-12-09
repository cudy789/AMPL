#!/bin/bash

# Corey Knutson, 12/2/2021

docker container prune -f
docker image prune -f

IMAGE_NAME="ampl"
IMAGE_TAG="latest"
PLATFORM="linux/arm64"

if [ "$BUILDX64" = "1" ]; then # build only for x86_64
  IMAGE_TAG="X64"
  PLATFORM="linux/amd64"
fi

# No cross compilation
docker build --platform $PLATFORM --tag rogueraptor7/$IMAGE_NAME:$IMAGE_TAG .
docker push rogueraptor7/$IMAGE_NAME:$IMAGE_TAG