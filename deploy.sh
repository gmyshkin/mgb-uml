#!/bin/bash
# Run this ONCE on the DigitalOcean Server to set up the environment.

echo "--- 🛠️ TikZiT Server Setup ---"

# 1. Login to Registry
echo "🔑 Logging into DigitalOcean Registry..."
echo "Please enter your DigitalOcean API Token (Hidden input):"
read -s DO_TOKEN
echo $DO_TOKEN | docker login registry.digitalocean.com -u _ --password-stdin

# 2. Create the 'tikzit_deployment' structure
# This is just a folder name we chose to keep your files organized.
echo "📂 Creating server directories..."
mkdir -p ~/tikzit_deployment/user_configs
mkdir -p ~/tikzit_deployment/tikzit_docs_vol

# 3. Create the Network
# The containers need a specific network to talk to each other.
echo "🌐 Creating Docker Network..."
docker network create tikzit_net || true

# 4. Create the Volume for Docs
echo "📚 Creating Docs Volume..."
docker volume create tikzit_docs

echo "✅ Setup Complete. You are ready to run docker compose up."