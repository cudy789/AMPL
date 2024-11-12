#!/bin/bash

# Corey Knutson, 12/2/2021

docker container prune -f
docker image prune -f


IMAGE_NAME="apriltag-multicam"
IMAGE_TAG="latest"

docker buildx create --use
docker buildx build --push --platform linux/arm64/v8,linux/amd64 --tag rogueraptor7/$IMAGE_NAME:$IMAGE_TAG .