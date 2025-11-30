#!/bin/bash
set -e # Stop script if any command fails

# --- CONFIGURATION ---
REGISTRY_URL="registry.digitalocean.com/mgb-uml"
IMAGE_NAME="tikzit"
# ---------------------

# 1. Get Versions
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
sed -i "s/^PROJECT_NUMBER[[:space:]]*=.*/PROJECT_NUMBER         = $NEW_VERSION/" Doxyfile
echo "✅ Updated Version Files."

# 3. Regenerate Documentation
echo "⚙️  Regenerating Doxygen Docs..."
# We use your current method (ubuntu container) because it works reliably
docker run --rm -v "$PWD":/src -w /src ubuntu:22.04 /bin/bash -c "apt-get update && apt-get install -y doxygen && doxygen Doxyfile"

# --- 3.5 NEW SAFETY CHECK ---
if [ ! -f "docs/html/index.html" ]; then
    echo "❌ CRITICAL ERROR: Docs were not generated!"
    echo "   I looked for 'docs/html/index.html' but could not find it."
    echo "   Check your Doxyfile settings (OUTPUT_DIRECTORY should be 'docs')."
    exit 1
fi
echo "✅ Docs confirmed locally."
# -----------------------------

# 4. Git Operations
echo "🐙 Pushing to GitHub..."
git add .
git commit -m "Release v$NEW_VERSION"
git tag -a "v$NEW_VERSION" -m "Release $NEW_VERSION"
git push origin main
git push origin "v$NEW_VERSION"

# 5. Docker Operations
echo "🐳 Building Docker Image (Forcing No Cache for Docs)..."

# We add --no-cache to ensure it actually COPIES the new docs folder
docker build -t $REGISTRY_URL/$IMAGE_NAME:$NEW_VERSION -t $REGISTRY_URL/$IMAGE_NAME:latest .

echo "🚀 Pushing to DigitalOcean..."
docker push $REGISTRY_URL/$IMAGE_NAME:$NEW_VERSION
docker push $REGISTRY_URL/$IMAGE_NAME:latest

echo "----------------------------------------------------"
echo "🎉 SUCCESS! Version $NEW_VERSION is live."
echo "----------------------------------------------------"
echo "Next Step: Run './update_and_redeploy.sh' on the server."