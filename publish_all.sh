#!/bin/bash
set -e

# --- CONFIGURATION ---
REGISTRY_URL="registry.digitalocean.com/mgb-uml"
IMAGE_NAME="tikzit"
# ---------------------

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

# --- NEW: Ask for a description of changes ---
echo "📝 What changed in this version?"
read -p "Description (e.g. 'Fixed PDF export bug'): " VERSION_DESC

if [ -z "$VERSION_DESC" ]; then
    VERSION_DESC="Updates for v$NEW_VERSION" # Default if you leave it blank
fi
# ---------------------------------------------

# 2. Update Version Files
echo "$NEW_VERSION" > VERSION
sed -i "s/^PROJECT_NUMBER[[:space:]]*=.*/PROJECT_NUMBER         = $NEW_VERSION/" Doxyfile
echo "✅ Updated Version Files."

# 3. Regenerate Documentation
echo "⚙️  Regenerating Doxygen Docs..."
docker run --rm -v "$PWD":/src -w /src ubuntu:22.04 /bin/bash -c "apt-get update && apt-get install -y doxygen && doxygen Doxyfile"

# 3.5 Safety Check
if [ ! -f "docs/html/index.html" ]; then
    echo "❌ CRITICAL ERROR: Docs were not generated!"
    exit 1
fi
echo "✅ Docs confirmed locally."

# 4. Git Operations
echo "🐙 Pushing to GitHub..."
git add .

# --- UPDATED: Use the description in the commit and tag ---
git commit -m "Release v$NEW_VERSION: $VERSION_DESC"
git tag -a "v$NEW_VERSION" -m "$VERSION_DESC"
# ----------------------------------------------------------

git push origin main
git push origin "v$NEW_VERSION"
echo "✅ Code and Tags pushed to GitHub."

# 5. Docker Operations
echo "🐳 Building Docker Image..."
docker build -t $REGISTRY_URL/$IMAGE_NAME:$NEW_VERSION -t $REGISTRY_URL/$IMAGE_NAME:latest .

echo "🚀 Pushing to DigitalOcean..."
docker push $REGISTRY_URL/$IMAGE_NAME:$NEW_VERSION
docker push $REGISTRY_URL/$IMAGE_NAME:latest

echo "----------------------------------------------------"
echo "🎉 SUCCESS! Version $NEW_VERSION is live."
echo "----------------------------------------------------"
echo "Next Step: Run './update_and_redeploy.sh' on the server."