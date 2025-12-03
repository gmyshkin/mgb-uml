#!/bin/bash
set -e

# --- CONFIGURATION ---
REGISTRY_URL="registry.digitalocean.com/mgb-uml"
IMAGE_NAME="tikzit"
LATEX_DIR="latex_documentation"
LATEX_MAIN="MGBProjectProposal.tex"
# ---------------------

echo "----------------------------------------------------"
echo "📦 TIKZIT PUBLISHING WIZARD (Local + Tests)"
echo "----------------------------------------------------"

# 1. RUN TESTS FIRST (Fail fast!)
echo "🧪 Running Python Unit Tests..."
if command -v pytest &> /dev/null; then
    # Run tests locally if pytest is installed
    pytest tests/deployment/ || { echo "❌ Tests Failed! Stopping release."; exit 1; }
else
    echo "⚠️  pytest not found locally. Running inside Docker..."
    # Run tests inside a temporary python container
    docker run --rm -v "$PWD":/app -w /app python:3.9 \
        /bin/bash -c "pip install -r tests/requirements.txt && pip install pytest && pytest tests/deployment/" \
        || { echo "❌ Tests Failed! Stopping release."; exit 1; }
fi
echo "✅ Tests Passed."

# 2. AUTO-INCREMENT VERSION
if [ -f VERSION ]; then CURRENT_VERSION=$(cat VERSION); else CURRENT_VERSION="0.0.0"; fi
IFS='.' read -r -a parts <<< "$CURRENT_VERSION"
next_patch=$((${parts[2]} + 1))
SUGGESTED_VERSION="${parts[0]}.${parts[1]}.$next_patch"

echo "Current Version: $CURRENT_VERSION"
read -p "Enter version (Press ENTER to use $SUGGESTED_VERSION): " INPUT_VERSION
NEW_VERSION="${INPUT_VERSION:-$SUGGESTED_VERSION}"

# Duplicate Check
if git rev-parse "v$NEW_VERSION" >/dev/null 2>&1; then
    echo "❌ ERROR: Version v$NEW_VERSION already exists!"
    exit 1
fi

GIT_HASH=$(git rev-parse --short HEAD)
echo "📝 What changed?"
read -p "Description: " USER_DESC
if [ -z "$USER_DESC" ]; then USER_DESC="Updates for v$NEW_VERSION"; fi
FINAL_MSG="$USER_DESC [Ref: $GIT_HASH]"

# 3. Update Version Files
echo "$NEW_VERSION" > VERSION
if [[ "$(uname)" == "Darwin" ]]; then
    sed -i '' "s/^PROJECT_NUMBER[[:space:]]*=.*/PROJECT_NUMBER         = $NEW_VERSION/" Doxyfile
else
    sed -i "s/^PROJECT_NUMBER[[:space:]]*=.*/PROJECT_NUMBER         = $NEW_VERSION/" Doxyfile
fi

# 4. Compile LaTeX
echo "📄 Compiling LaTeX..."
docker run --rm --platform linux/amd64 -v "$PWD":/data -w /data/$LATEX_DIR grandline/latex pdflatex -interaction=nonstopmode $LATEX_MAIN > /dev/null 2>&1 || true

# 5. Generate Doxygen
echo "⚙️  Regenerating Doxygen..."
docker run --rm --platform linux/amd64 -v "$PWD":/src -w /src ubuntu:22.04 /bin/bash -c "apt-get update && apt-get install -y doxygen && doxygen Doxyfile"
if [ ! -f "docs/html/index.html" ]; then echo "❌ Doxygen Failed!"; exit 1; fi

# 6. Create ZIP Artifact
echo "🗜️  Creating ZIP..."
ZIP_NAME="tikzit-release-v$NEW_VERSION-$GIT_HASH.zip"
# Updated to include the new latex folder location
zip -r $ZIP_NAME src/ docs/ $LATEX_DIR/*.pdf VERSION nginx.conf *.sh > /dev/null || true

# 7. Git Operations
echo "🐙 Pushing to GitHub..."
git add .
git commit -m "Release v$NEW_VERSION: $FINAL_MSG"
git tag -a "v$NEW_VERSION" -m "$FINAL_MSG"
git push origin main
git push origin "v$NEW_VERSION"

# 8. Docker Build & Push
echo "🐳 Building Docker Image..."
docker build --platform linux/amd64 \
  -t $REGISTRY_URL/$IMAGE_NAME:$NEW_VERSION \
  -t $REGISTRY_URL/$IMAGE_NAME:latest \
  -t $REGISTRY_URL/$IMAGE_NAME:git-$GIT_HASH \
  .

echo "🚀 Pushing to DigitalOcean..."
docker push $REGISTRY_URL/$IMAGE_NAME:$NEW_VERSION
docker push $REGISTRY_URL/$IMAGE_NAME:latest

echo "----------------------------------------------------"
echo "🎉 SUCCESS! v$NEW_VERSION is Live."
echo "----------------------------------------------------"