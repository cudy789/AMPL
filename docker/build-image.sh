#!/bin/bash

# Corey Knutson, 12/2/2021

docker container prune -f
docker image prune -f


if [ "$NO_CACHE" = "1" ]; then
  echo "building image without cache"
  CACHE_FLAG="--no-cache"
else
  echo "building image using cache"
  CACHE_FLAG=""
fi

if [ "$BUILDX" = "0" ]; then
  BUILDX=0
else
  BUILDX=1
fi

IMAGE_NAME="apriltag-multicam"
IMAGE_TAG="latest"

if [ "$BUILDX" = "1" ]; then # build for both arm and x86 architectures, push image to dockerhub
  docker buildx build $CACHE_FLAG --push --platform linux/arm64/v8,linux/amd64 --tag rogueraptor7/$IMAGE_NAME:$IMAGE_TAG .
elif [ "$BUILDARM" = "1" ]; then # build only for arm, push image to dockerhub
  docker buildx build $CACHE_FLAG --push --platform linux/arm64/v8 --tag rogueraptor7/$IMAGE_NAME:$IMAGE_TAG .
elif [ "$NATIVEARM" = "1" ]; then # build only for arm, push image to dockerhub
  docker build $CACHE_FLAG --tag rogueraptor7/$IMAGE_NAME:$IMAGE_TAG .
  docker push rogueraptor7/$IMAGE_NAME:$IMAGE_TAG
elif [ "$BUILDX86" = "1" ]; then # build only for x86, push image to dockerhub
  docker buildx build $CACHE_FLAG --push --platform linux/amd64 --tag rogueraptor7/$IMAGE_NAME:$IMAGE_TAG .
else # build for native architecture, don't push to dockerhub
  echo "building rogueraptor7/$IMAGE_NAME:$IMAGE_TAG"
  docker build -t rogueraptor7/$IMAGE_NAME:$IMAGE_TAG .
fi
