#!/bin/bash
set -e

# --- CONFIGURATION ---
REGISTRY_URL="registry.digitalocean.com/mgb-uml"
IMAGE_NAME="tikzit"
LATEX_DIR="latex_documentation"
LATEX_MAIN="itManual.tex"  # Updated to your correct main file
# ---------------------

echo "----------------------------------------------------"
echo "🤖 TIKZIT AUTO-PILOT RELEASE"
echo "----------------------------------------------------"

# 1. RUN TESTS FIRST
echo "🧪 Running Python Unit Tests..."
if command -v pytest &> /dev/null; then
    pytest tests/deployment/ || { echo "❌ Tests Failed!"; exit 1; }
else
    docker run --rm -v "$PWD":/app -w /app python:3.9 \
        /bin/bash -c "pip install -r tests/requirements.txt && pip install pytest && pytest tests/deployment/" \
        || { echo "❌ Tests Failed!"; exit 1; }
fi
echo "✅ Tests Passed."

# 2. AUTO-INCREMENT VERSION
if [ -f VERSION ]; then CURRENT_VERSION=$(cat VERSION); else CURRENT_VERSION="0.0.0"; fi
IFS='.' read -r -a parts <<< "$CURRENT_VERSION"
next_patch=$((${parts[2]} + 1))
NEW_VERSION="${parts[0]}.${parts[1]}.$next_patch"

# Collision check
while git rev-parse "v$NEW_VERSION" >/dev/null 2>&1; do
    next_patch=$((next_patch + 1))
    NEW_VERSION="${parts[0]}.${parts[1]}.$next_patch"
done

echo "🔹 Target Version: $NEW_VERSION"

# 3. HANDLE UNCOMMITTED CODE (The Hash Fix)
if [[ -n $(git status -s) ]]; then
    echo "📝 Found uncommitted changes. Committing them now..."
    read -p "   Commit message for code changes: " WORK_MSG
    if [ -z "$WORK_MSG" ]; then WORK_MSG="Updates before v$NEW_VERSION"; fi
    git add .
    git commit -m "$WORK_MSG"
    echo "✅ Code committed."
fi

# 4. CAPTURE THE HASH
GIT_HASH=$(git rev-parse --short HEAD)
echo "🔗 Hash Locked: $GIT_HASH"

# 5. UPDATE VERSION FILES
echo "$NEW_VERSION" > VERSION
sed -i "s/^PROJECT_NUMBER[[:space:]]*=.*/PROJECT_NUMBER         = $NEW_VERSION/" Doxyfile

# 6. GENERATE ARTIFACTS
echo "📄 Compiling LaTeX..."
docker run --rm --platform linux/amd64 -v "$PWD":/data -w /data/$LATEX_DIR grandline/latex pdflatex -interaction=nonstopmode $LATEX_MAIN > /dev/null 2>&1 || true

echo "⚙️  Regenerating Doxygen..."
docker run --rm --platform linux/amd64 -v "$PWD":/src -w /src ubuntu:22.04 /bin/bash -c "apt-get update && apt-get install -y doxygen && doxygen Doxyfile"
if [ ! -f "docs/html/index.html" ]; then echo "❌ Doxygen Failed!"; exit 1; fi

echo "🗜️  Creating ZIP..."
ZIP_NAME="tikzit-release-v$NEW_VERSION-$GIT_HASH.zip"
zip -r $ZIP_NAME src/ docs/ $LATEX_DIR/*.pdf VERSION nginx.conf *.sh > /dev/null || true

# 7. GIT RELEASE (The Tag)
echo "🐙 Tagging Release..."
git add .
RELEASE_MSG="Release v$NEW_VERSION [Ref: $GIT_HASH]"
git commit -m "$RELEASE_MSG"
git tag -a "v$NEW_VERSION" -m "$RELEASE_MSG"
git push origin main
git push origin "v$NEW_VERSION"

# 8. DOCKER DEPLOY
echo "🐳 Building & Pushing..."
docker build --platform linux/amd64 \
  -t $REGISTRY_URL/$IMAGE_NAME:$NEW_VERSION \
  -t $REGISTRY_URL/$IMAGE_NAME:latest \
  -t $REGISTRY_URL/$IMAGE_NAME:git-$GIT_HASH \
  .
docker push $REGISTRY_URL/$IMAGE_NAME:$NEW_VERSION
docker push $REGISTRY_URL/$IMAGE_NAME:latest

echo "🎉 DONE: v$NEW_VERSION [Ref: $GIT_HASH]"
#Comment to show a change in code