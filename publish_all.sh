#!/bin/bash
set -e

# --- CONFIGURATION ---
REGISTRY_URL="registry.digitalocean.com/mgb-uml"
IMAGE_NAME="tikzit"
# ---------------------

echo "----------------------------------------------------"
echo "🤖 TIKZIT AUTO-PILOT RELEASE"
echo "----------------------------------------------------"

# 1. AUTO-INCREMENT VERSION
if [ -f VERSION ]; then CURRENT_VERSION=$(cat VERSION); else CURRENT_VERSION="0.0.0"; fi

# Split version (1.0.0) into parts
IFS='.' read -r -a parts <<< "$CURRENT_VERSION"
major=${parts[0]}
minor=${parts[1]}
patch=${parts[2]}

# Logic: Increment Patch
next_patch=$((patch + 1))
NEW_VERSION="$major.$minor.$next_patch"

# Safety: Check if tag exists (Collision detection)
while git rev-parse "v$NEW_VERSION" >/dev/null 2>&1; do
    echo "⚠️  v$NEW_VERSION already exists. Skipping..."
    next_patch=$((next_patch + 1))
    NEW_VERSION="$major.$minor.$next_patch"
done

echo "🔹 Current Version: $CURRENT_VERSION"
echo "🔹 Target Version:  $NEW_VERSION"
echo "   (Auto-calculated to prevent duplicates)"
echo "----------------------------------------------------"

# 2. HANDLE UNCOMMITTED CODE (The Hash Fix)
# We check if there are changed files that haven't been committed yet
if [[ -n $(git status -s) ]]; then
    echo "📝 Found uncommitted changes in your code."
    echo "   I need to commit these FIRST to get the correct Hash."
    read -p "   Enter commit message for your code changes: " WORK_MSG
    
    if [ -z "$WORK_MSG" ]; then WORK_MSG="Code updates before release v$NEW_VERSION"; fi
    
    git add .
    git commit -m "$WORK_MSG"
    echo "✅ Code committed."
else
    echo "✅ Clean working tree. Proceeding..."
fi

# 3. CAPTURE THE HASH (Now it's the correct one!)
GIT_HASH=$(git rev-parse --short HEAD)
echo "🔗 Locking in Source Hash: $GIT_HASH"

# 4. UPDATE VERSION FILES
echo "$NEW_VERSION" > VERSION
if [[ "$(uname)" == "Darwin" ]]; then
    sed -i '' "s/^PROJECT_NUMBER[[:space:]]*=.*/PROJECT_NUMBER         = $NEW_VERSION/" Doxyfile
else
    sed -i "s/^PROJECT_NUMBER[[:space:]]*=.*/PROJECT_NUMBER         = $NEW_VERSION/" Doxyfile
fi

# 5. GENERATE ARTIFACTS
echo "📄 Compiling LaTeX..."
docker run --rm --platform linux/amd64 -v "$PWD":/data -w /data grandline/latex pdflatex -interaction=nonstopmode project_report.tex > /dev/null 2>&1 || true

echo "⚙️  Regenerating Doxygen..."
docker run --rm --platform linux/amd64 -v "$PWD":/src -w /src ubuntu:22.04 /bin/bash -c "apt-get update && apt-get install -y doxygen && doxygen Doxyfile" || true

echo "🗜️  Creating ZIP..."
ZIP_NAME="tikzit-release-v$NEW_VERSION-$GIT_HASH.zip"
zip -r $ZIP_NAME src/ docs/ project_report.pdf VERSION nginx.conf *.sh > /dev/null || true

# 6. GIT RELEASE (The Final Tag)
echo "🐙 Tagging Release on GitHub..."
git add .
# We use [skip ci] to prevent infinite loops if you set up Actions later
RELEASE_MSG="Release v$NEW_VERSION [Ref: $GIT_HASH]"

git commit -m "$RELEASE_MSG"
git tag -a "v$NEW_VERSION" -m "$RELEASE_MSG"

git push origin main
git push origin "v$NEW_VERSION"

# 7. DOCKER DEPLOY
echo "🐳 Building & Pushing Docker Image..."
docker build --platform linux/amd64 \
  -t $REGISTRY_URL/$IMAGE_NAME:$NEW_VERSION \
  -t $REGISTRY_URL/$IMAGE_NAME:latest \
  -t $REGISTRY_URL/$IMAGE_NAME:git-$GIT_HASH \
  .

docker push $REGISTRY_URL/$IMAGE_NAME:$NEW_VERSION
docker push $REGISTRY_URL/$IMAGE_NAME:latest

echo "----------------------------------------------------"
echo "🎉 DEPLOYMENT COMPLETE: v$NEW_VERSION"
echo "🔗 Hash Ref: $GIT_HASH"
echo "----------------------------------------------------"
#Test with this publish_all scrip, trying to make sure everything works fine.