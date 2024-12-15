#!/bin/bash

echo "#################################################################################################"
echo "Setting up a static IP for your computer using NetworkManager CLI. The default IP is 10.TE.AM.15"

# Function to validate IP address format
validate_ip() {
    local ip="$1"
    if [[ "$ip" =~ ^10\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,2}$ ]]; then
        return 0
    else
        return 1
    fi
}

# Function to configure static IP with NetworkManager
configure_static_ip() {
    local ip="$1"
    local interface="$2"
    local connection_name="AMPL connection"
    local detected_connection_name=$(nmcli -t -f NAME,DEVICE con show | grep "$connection_name" | cut -d: -f1)

    if [[ -z "$interface" ]]; then
        echo "Error: Could not detect active network interface."
        exit 1
    elif [[ -z "$detected_connection_name" ]]; then
        echo "Adding $connection_name to interface $interface"
        sudo nmcli con add type ethernet con-name "$connection_name" ifname "$interface"
    fi

    echo "Configuring static IP $ip on interface $interface (connection: $connection_name)..."
    sudo nmcli con mod "$connection_name" \
        ipv4.addresses "$ip/24" \
        ipv4.gateway "${STATIC_IP%.*}.1" \
        ipv4.dns "" \
        ipv4.method manual

    echo "Restarting NetworkManager connection..."
    sudo nmcli con up "$connection_name"

    if [[ $? -eq 0 ]]; then
        echo "Static IP $ip successfully configured for connection $connection_name."
    else
        echo "Failed to apply static IP configuration."
        exit 1
    fi
}

# Main logic
if [[ -z "$INTERFACE" ]]; then
  INTERFACE="eth0"
fi

if [[ -n "$STATIC_IP" ]]; then
    # Non-interactive mode
    if validate_ip "$STATIC_IP"; then
        configure_static_ip "$STATIC_IP" "$INTERFACE"
    else
        echo "Error: Invalid STATIC_IP value: $STATIC_IP"
        exit 1
    fi
else
    # Interactive mode
    read -p "Enter your team number (4 or 5 digits): " team_number

    if [[ "$team_number" =~ ^[0-9]{4}$ ]]; then
        WX="${team_number:0:2}"
        YZ="${team_number:2:2}"
        STATIC_IP="10.$WX.$YZ.15"
    elif [[ "$team_number" =~ ^[0-9]{5}$ ]]; then
        VWX="${team_number:0:3}"
        YZ="${team_number:3:2}"
        STATIC_IP="10.$VWX.$YZ.15"
    else
        echo "Error: Team number must be 4 or 5 digits."
        exit 1
    fi

    if validate_ip "$STATIC_IP"; then
        read -p "$STATIC_IP will be set on interface $INTERFACE, are you sure you want to continue (y/N)? " set_static_ip
        if [[ "$set_static_ip" != "y" ]]; then
          echo "Operation canceled by user."
          exit 0
        fi

        configure_static_ip "$STATIC_IP" "$INTERFACE"
    else
        echo "Error: Computed IP is invalid: $STATIC_IP"
        exit 1
    fi
fi

echo "#################################################################################################"