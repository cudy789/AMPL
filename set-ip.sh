#!/bin/bash

echo "Setting up a static IP for your computer. The default IP is 10.TE.AM.15"

# Check if STATIC_IP is set in the CLI
if [[ -z "$STATIC_IP" ]]; then
    # Prompt the user for the team number if STATIC_IP is not set
    echo -n "Enter the team number (4 or 5 digits): "
    read TEAM_NUMBER

    # Validate the provided team number
    if [[ ! "$TEAM_NUMBER" =~ ^[0-9]{4,5}$ ]]; then
        echo "Error: Invalid team number '$TEAM_NUMBER'. Please provide a valid 4 or 5-digit team number."
        exit 1
    fi

    # Calculate the IP address based on the length of the team number
    if [[ ${#TEAM_NUMBER} -eq 4 ]]; then
        # For 4-digit team numbers (WXYZ -> 10.WX.YZ.15)
        WX=${TEAM_NUMBER:0:2}
        YZ=${TEAM_NUMBER:2:2}
        STATIC_IP="10.$WX.$YZ.15"
    elif [[ ${#TEAM_NUMBER} -eq 5 ]]; then
        # For 5-digit team numbers (VWXYZ -> 10.VWX.YZ.15)
        VWX=${TEAM_NUMBER:0:3}
        YZ=${TEAM_NUMBER:3:2}
        STATIC_IP="10.$VWX.$YZ.15"
    fi
fi

# Validate the calculated or overridden IP address
if [[ ! "$STATIC_IP" =~ ^10\.[0-9]+\.[0-9]+\.[0-9]+$ ]]; then
    echo "Error: Invalid static IP '$STATIC_IP'. Please provide a valid IP address."
    exit 1
fi

# Define the network configuration file
NET_CONFIG_FILE="/etc/dhcpcd.conf"

# Backup the current network configuration
sudo cp "$NET_CONFIG_FILE" "${NET_CONFIG_FILE}.bak"

# Add static IP configuration to dhcpcd.conf
echo "Setting static IP $STATIC_IP..."

sudo bash -c "cat >> $NET_CONFIG_FILE <<EOL

# Static IP configuration
interface eth0
static ip_address=$STATIC_IP/24
static routers=${STATIC_IP%.*}.1
static domain_name_servers=8.8.8.8 8.8.4.4
EOL"

# Restart the DHCP client service
sudo systemctl restart dhcpcd

echo "Static IP $STATIC_IP has been set!"
