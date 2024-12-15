#!/bin/bash

# Function to check if a command exists
command_exists() {
    command -v "$1" &> /dev/null
}

echo "Checking for Docker installation..."

# Check if Docker is installed
if command_exists docker; then
    echo "Docker is already installed."
else
    echo "Docker is not installed. Proceeding with installation..."

    # Update package index
    sudo apt update

    # Install prerequisites
    sudo apt install -y \
        apt-transport-https \
        ca-certificates \
        curl \
        software-properties-common

    # Update package index again
    sudo apt update

    # Install Docker Engine
    sudo apt install -y docker.io

    # Enable and start Docker service
    sudo systemctl enable docker
    sudo systemctl start docker

    echo "Docker has been installed successfully."
fi

echo "Checking for Docker Compose installation..."

# Check if Docker Compose is installed
if command_exists docker compose; then
    echo "Docker Compose is already installed."
else
    echo "Docker Compose is not installed. Proceeding with installation..."

    # Update package index
    sudo apt update

    # Install docker compose
    sudo apt install docker-compose

    # Verify installation
    if command_exists docker compose; then
        echo "Docker Compose has been installed successfully."
    else
        echo "There was an issue installing Docker Compose."
    fi
fi

# Make config directory
mkdir -p /home/"$USER"/ampl-config
cd /home/"$USER"/ampl-config

# Get config files
wget https://raw.githubusercontent.com/cudy789/AMPL/refs/heads/main/config.yml
wget https://raw.githubusercontent.com/cudy789/AMPL/refs/heads/main/docker-compose.yml

# Download latest docker images
docker compose pull

# Setup the static IP
if [[ -z "$SKIP_STATIC_IP" ]]; then
  wget https://raw.githubusercontent.com/cudy789/AMPL/refs/heads/main/set-ip.sh
  sudo bash -i set-ip.sh
else
  echo "Skipping static IP configuration"
fi

echo "Installation complete! Modify your config.yml file for your specific setup, then run 'docker compose up -d' to start AMPL."
