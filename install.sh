#!/bin/bash

echo "Installing Docker and docker-compose"

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

# Setup the static IP
if [[ -z "$SKIP_STATIC_IP" ]]; then
  wget https://raw.githubusercontent.com/cudy789/AMPL/refs/heads/main/set-ip.sh
  sudo bash -i set-ip.sh
else
  echo "Skipping static IP configuration"
fi

echo "############ Installation is almost complete! ############

To finish the installation:

1. Log out of your terminal and log back in
2. Go to the ~/ampl-config directory and run 'docker-compose pull'

Then, to configure and start AMPL:

1. Modify your ~/ample-config/config.yml file for your specific camera setup
2. Run 'docker-compose up -d' to start AMPL

##########################################################
"
