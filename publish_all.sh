#!/bin/bash
# Removed 'set -e' so the script doesn't crash if a PDF fails to generate
# set -e 

# --- CONFIGURATION ---
REGISTRY_URL="registry.digitalocean.com/mgb-uml"
IMAGE_NAME="tikzit"
# ---------------------

# 1. Setup Version
OS_TYPE=$(uname)
if [ -f VERSION ]; then CURRENT_VERSION=$(cat VERSION); else CURRENT_VERSION="0.0.0"; fi

echo "----------------------------------------------------"
echo "📦 TIKZIT PUBLISHING WIZARD (Debug Mode)"
echo "----------------------------------------------------"
echo "Current Version: $CURRENT_VERSION"
read -p "Enter NEW version (e.g., 1.0.2): " NEW_VERSION

if [ -z "$NEW_VERSION" ]; then echo "❌ No version. Exiting."; exit 1; fi

# Update Version File
echo "$NEW_VERSION" > VERSION
if [[ "$OS_TYPE" == "Darwin" ]]; then
    sed -i '' "s/^PROJECT_NUMBER[[:space:]]*=.*/PROJECT_NUMBER         = $NEW_VERSION/" Doxyfile
else
    sed -i "s/^PROJECT_NUMBER[[:space:]]*=.*/PROJECT_NUMBER         = $NEW_VERSION/" Doxyfile
fi
echo "✅ Version bumped."

# 2. Compile LaTeX (With Error Handling)
echo "📄 Compiling LaTeX..."
# Added '|| true' so the script continues even if PDF generation fails
docker run --rm --platform linux/amd64 -v "$PWD":/data -w /data grandline/latex pdflatex -interaction=nonstopmode project_report.tex > /dev/null 2>&1 || echo "⚠️ LaTeX compilation had warnings/errors (Check project_report.tex)"

if [ ! -f "project_report.pdf" ]; then
    echo "⚠️ WARNING: project_report.pdf was NOT created. Creating a placeholder..."
    touch project_report.pdf
fi

# 3. Generate Doxygen (With Error Handling)
echo "⚙️  Regenerating Doxygen..."
docker run --rm --platform linux/amd64 -v "$PWD":/src -w /src ubuntu:22.04 /bin/bash -c "apt-get update && apt-get install -y doxygen && doxygen Doxyfile" || true

if [ ! -f "docs/html/index.html" ]; then
    echo "❌ CRITICAL: Doxygen failed. Index.html missing."
    # We exit here because the web app NEEDS this file
    exit 1
fi

# 4. Create ZIP
echo "🗜️  Creating ZIP Artifact..."
GIT_HASH=$(git rev-parse --short HEAD)
ZIP_NAME="tikzit-release-v$NEW_VERSION-$GIT_HASH.zip"
zip -r $ZIP_NAME src/ docs/ project_report.pdf VERSION nginx.conf *.sh > /dev/null || true

# 5. Git Push (Explicit)
echo "----------------------------------------------------"
echo "🐙 PREPARING TO PUSH TO GITHUB..."
echo "   Commit Message: Release v$NEW_VERSION"
echo "----------------------------------------------------"

git add .
git commit -m "Release v$NEW_VERSION [Ref: $GIT_HASH]"
git tag -a "v$NEW_VERSION" -m "Release $NEW_VERSION"

echo "⬆️  Pushing Code to GitHub (Main)..."
git push origin main

echo "⬆️  Pushing Tags to GitHub..."
git push origin "v$NEW_VERSION"

# 6. Docker Push (Explicit)
echo "----------------------------------------------------"
echo "🐳 PREPARING DOCKER BUILD & PUSH..."
echo "----------------------------------------------------"

# Force AMD64 platform for DigitalOcean compatibility
docker build --platform linux/amd64 \
  -t $REGISTRY_URL/$IMAGE_NAME:$NEW_VERSION \
  -t $REGISTRY_URL/$IMAGE_NAME:latest \
  .

echo "⬆️  Pushing Image: $NEW_VERSION..."
docker push $REGISTRY_URL/$IMAGE_NAME:$NEW_VERSION

echo "⬆️  Pushing Image: latest..."
docker push $REGISTRY_URL/$IMAGE_NAME:latest

echo "----------------------------------------------------"
echo "🎉 DONE! Check your GitHub and DigitalOcean."
echo "----------------------------------------------------"