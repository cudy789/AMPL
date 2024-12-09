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
    sudo apt update -y

    # Install prerequisites
    sudo apt install -y \
        apt-transport-https \
        ca-certificates \
        curl \
        software-properties-common

    # Add Docker's official GPG key
    curl -fsSL https://download.docker.com/linux/ubuntu/gpg | sudo gpg --dearmor -o /usr/share/keyrings/docker-archive-keyring.gpg

    # Set up the stable repository
    echo \
        "deb [arch=$(dpkg --print-architecture) signed-by=/usr/share/keyrings/docker-archive-keyring.gpg] https://download.docker.com/linux/ubuntu \
        $(lsb_release -cs) stable" | sudo tee /etc/apt/sources.list.d/docker.list > /dev/null

    # Update package index again
    sudo apt update -y

    # Install Docker Engine
    sudo apt install -y docker-ce docker-ce-cli containerd.io

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

    # Download the current stable release of Docker Compose
    sudo curl -L "https://github.com/docker/compose/releases/latest/download/docker-compose-$(uname -s)-$(uname -m)" -o /usr/local/bin/docker-compose

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
