#!/bin/bash
# Run this ON THE SERVER to grab the new update

REGISTRY_URL="registry.digitalocean.com/mgb-uml/tikzit:latest"

echo "⬇️ Force-Pulling latest image from Registry..."
docker pull $REGISTRY_URL

echo "📚 Updating Documentation (Extracting from Image to Volume)..."
# This takes the HTML files inside the new image and copies them into the Nginx volume
docker run --rm \
  -v tikzit_docs:/target \
  $REGISTRY_URL \
  cp -r /src/docs/html/. /target/

echo "🔄 Restarting Infrastructure (Nginx & Default Container)..."
docker compose up -d --force-recreate

echo "✅ Update Complete! Docs and Default App are live."