#!/bin/bash
set -e # Stop script if any command fails

# --- CONFIGURATION (CHANGE THESE) ---
REGISTRY_URL="registry.digitalocean.com/mgb-uml"
IMAGE_NAME="tikzit"
# ------------------------------------

# 1. Get Current and New Version
if [ -f VERSION ]; then
    CURRENT_VERSION=$(cat VERSION)
else
    CURRENT_VERSION="0.0.0"
fi

echo "----------------------------------------------------"
echo "📦 TIKZIT PUBLISHING WIZARD"
echo "----------------------------------------------------"
echo "Current Version: $CURRENT_VERSION"
read -p "Enter NEW version (e.g., 1.0.1): " NEW_VERSION

if [ -z "$NEW_VERSION" ]; then
    echo "❌ No version entered. Exiting."
    exit 1
fi

# 2. Update Version Files
echo "$NEW_VERSION" > VERSION
echo "✅ Updated VERSION file."

# Update Doxyfile project number
sed -i "s/^PROJECT_NUMBER[[:space:]]*=.*/PROJECT_NUMBER         = $NEW_VERSION/" Doxyfile
echo "✅ Updated Doxyfile version."

# 3. Regenerate Documentation
echo "⚙️  Regenerating Doxygen Docs..."
# We run doxygen using the 'builder' stage logic locally to ensure docs are fresh
docker run --rm -v "$PWD":/src -w /src ubuntu:22.04 /bin/bash -c "apt-get update && apt-get install -y doxygen && doxygen Doxyfile"
echo "✅ Docs updated."

# 4. Git Operations
echo "🐙 Pushing to GitHub..."
git add .
git commit -m "Release v$NEW_VERSION"
git tag -a "v$NEW_VERSION" -m "Release $NEW_VERSION"
git push origin main
git push origin "v$NEW_VERSION"
echo "✅ Code and Tags pushed to GitHub."

# 5. Docker Operations
echo "🐳 Building and Pushing Docker Image..."
echo "   (This may take a moment...)"

# Build with two tags: specific version AND latest
docker build -t $REGISTRY_URL/$IMAGE_NAME:$NEW_VERSION -t $REGISTRY_URL/$IMAGE_NAME:latest .

# Push both to DigitalOcean
docker push $REGISTRY_URL/$IMAGE_NAME:$NEW_VERSION
docker push $REGISTRY_URL/$IMAGE_NAME:latest

echo "----------------------------------------------------"
echo "🎉 SUCCESS! Version $NEW_VERSION is live."
echo "----------------------------------------------------"
echo "Next Step: SSH into your server and run './update_and_redeploy.sh'"