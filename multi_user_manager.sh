#!/bin/bash

# CONFIGURATION
IMAGE_NAME="registry.digitalocean.com/mgb-uml/tikzit:latest"
NETWORK_NAME="tikzit_net"
CONFIG_DIR="./user_configs"

if [ -f .env ]; then
    # This reads the .env file and makes the variables usable in this script
    export $(cat .env | xargs)
else
    echo "❌ Error: .env file not found. Cannot start container without password."
    exit 1
fi


# 1. Ask for Username
echo "👤 TIKZIT USER CREATOR"
read -p "Enter username (lowercase, no spaces): " USERNAME

# Validation
if [[ ! "$USERNAME" =~ ^[a-z0-9]+$ ]]; then
    echo "❌ Invalid username. Use lowercase letters and numbers only."
    exit 1
fi

CONTAINER_NAME="tikzit_$USERNAME"

# 2. Check if user already exists
if docker ps -a --format '{{.Names}}' | grep -q "^${CONTAINER_NAME}$"; then
    echo "⚠️  User '$USERNAME' already exists."
    read -p "Do you want to DELETE and RECREATE this user? (y/n): " CONFIRM
    if [ "$CONFIRM" != "y" ]; then exit 0; fi
    
    echo "Stopping old container..."
    docker stop $CONTAINER_NAME && docker rm $CONTAINER_NAME
fi

# 3. Start the User Container
# We mount a unique volume for their data so they don't overwrite others
echo "🚀 Starting container for $USERNAME..."
docker run -d \
  --name $CONTAINER_NAME \
  --network $NETWORK_NAME \
  --restart always \
  -e VNC_PASSWORD=$VNC_PASSWORD \
  -v "tikzit_data_$USERNAME:/home/tikzituser/.local/share/tikzit" \
  $IMAGE_NAME

# 4. Create Nginx Routing Config
# This proxies domain.com/username/ -> container:8080/vnc.html
echo "🔗 configuring Nginx route..."

cat > "$CONFIG_DIR/$USERNAME.conf" <<EOF
location /$USERNAME/ {
    # The trailing slash after 8080 is CRITICAL. It strips /$USERNAME from the request.
    proxy_pass http://$CONTAINER_NAME:8080/;
    
    proxy_http_version 1.1;
    proxy_set_header Upgrade \$http_upgrade;
    proxy_set_header Connection "Upgrade";
    proxy_set_header Host \$host;
    
    # Rewrite cookie path so authentication works on subpath
    proxy_cookie_path / /$USERNAME/;
}
EOF

# 5. Reload Nginx
echo "🔄 Reloading Proxy..."
docker exec tikzit_proxy nginx -s reload

echo "------------------------------------------------"
echo "✅ SUCCESS!"
echo "Access URL: http://<YOUR_IP>/$USERNAME/vnc.html"
echo "------------------------------------------------"