#!/bin/bash

# Function to check if a command exists
command_exists() {
    command -v "$1" &> /dev/null
}

echo "Installing Docker and Docker Compose"

# Update package index
sudo apt update

# Install Docker Engine and Docker Compose
sudo apt install -y docker.io docker-compose

# Enable and start Docker service
sudo systemctl enable docker
sudo systemctl start docker

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
