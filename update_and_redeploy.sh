#!/bin/bash
# Run this ON THE SERVER to grab the new update

echo "⬇️ Pulling latest image from Registry..."
docker-compose pull

echo "🔄 Restarting container with new image..."
docker-compose up -d

echo "✅ Update Complete!"