#!/bin/bash

# Corey Knutson, 1/19/2025

# Since the script is not running in a VM/docker container, packages will always be installed prior to the script
# attempting to install them.
echo "Testing installation script for a fresh install. Skips network configuration."

wget -O install-maple.sh https://raw.githubusercontent.com/cudy789/MAPLE/refs/heads/main/install.sh

SKIP_STATIC_IP=1 bash install-maple.sh

cd maple-config

COMPOSE_CMD="docker compose"
# Append a user argument so logfiles aren't owned by root
echo "    user: $USER" >> docker-compose.yml
$COMPOSE_CMD pull

echo "Starting MAPLE"
timeout 30 "USER=$USER $COMPOSE_CMD up"
$COMPOSE_CMD down