#!/bin/bash
set -e

# --- CONFIGURATION ---
REGISTRY_URL="registry.digitalocean.com/mgb-uml"
IMAGE_NAME="tikzit"
LATEX_DIR="latex_documentation"
MAIN_TEX_FILE="MGBProjectProposal.tex" # Your main file identified above
# ---------------------

echo "----------------------------------------------------"
echo "🚀 TIKZIT LOCAL PIPELINE (Mirrors GitHub Actions)"
echo "----------------------------------------------------"

# 1. VERSIONING
if [ -f VERSION ]; then CURRENT_VERSION=$(cat VERSION); else CURRENT_VERSION="0.0.0"; fi
# Auto-increment logic
IFS='.' read -r -a parts <<< "$CURRENT_VERSION"
next_patch=$((${parts[2]} + 1))
SUGGESTED_VERSION="${parts[0]}.${parts[1]}.$next_patch"

echo "🔹 Current: $CURRENT_VERSION"
read -p "🔹 Enter Version (Press ENTER for $SUGGESTED_VERSION): " INPUT_VERSION
NEW_VERSION="${INPUT_VERSION:-$SUGGESTED_VERSION}"

# 2. RUN PYTHON TESTS (Matches GitHub Action)
echo "🐍 Running Python Tests (via Docker)..."
# We mount the current folder to /app and run pytest
docker run --rm -v "$PWD":/app -w /app python:3.9-slim /bin/bash -c "pip install -r tests/requirements.txt && pip install pytest && pytest tests/deployment/ --junitxml=test-results.xml"
echo "✅ Tests Passed."

# 3. COMPILE LATEX (Matches GitHub Action)
echo "📄 Compiling LaTeX Report (via Docker)..."
# We mount the root to /data, but set workdir to the latex folder
docker run --rm --platform linux/amd64 -v "$PWD":/data -w /data/$LATEX_DIR grandline/latex pdflatex -interaction=nonstopmode $MAIN_TEX_FILE > /dev/null 2>&1 || true

# Check if PDF was created
if [ -f "$LATEX_DIR/${MAIN_TEX_FILE%.*}.pdf" ]; then
    echo "✅ LaTeX Compiled Successfully."
    # Move PDF to root so we can zip it easily
    mv "$LATEX_DIR/${MAIN_TEX_FILE%.*}.pdf" project_report.pdf
else
    echo "⚠️ LaTeX Compilation Warning: PDF not found."
    touch project_report.pdf # Create placeholder so script doesn't crash
fi

# 4. UPDATE VERSION FILES
echo "$NEW_VERSION" > VERSION
# Mac/Linux sed compatibility
if [[ "$(uname)" == "Darwin" ]]; then
    sed -i '' "s/^PROJECT_NUMBER[[:space:]]*=.*/PROJECT_NUMBER         = $NEW_VERSION/" Doxyfile
else
    sed -i "s/^PROJECT_NUMBER[[:space:]]*=.*/PROJECT_NUMBER         = $NEW_VERSION/" Doxyfile
fi

# 5. GENERATE DOXYGEN
echo "⚙️  Regenerating Doxygen..."
docker run --rm --platform linux/amd64 -v "$PWD":/src -w /src ubuntu:22.04 /bin/bash -c "apt-get update && apt-get install -y doxygen graphviz && doxygen Doxyfile" > /dev/null 2>&1
if [ ! -f "docs/html/index.html" ]; then echo "❌ Doxygen Failed!"; exit 1; fi

# 6. CREATE ZIP ARTIFACT
echo "🗜️  Creating Deployment Artifact..."
GIT_HASH=$(git rev-parse --short HEAD)
ZIP_NAME="tikzit-release-v$NEW_VERSION-$GIT_HASH.zip"
zip -r $ZIP_NAME src/ docs/ project_report.pdf VERSION nginx.conf *.sh > /dev/null || true
echo "✅ Artifact: $ZIP_NAME"

# 7. GIT PUSH (Triggers GitHub Actions!)
echo "🐙 Pushing to GitHub..."
git add .
COMMIT_MSG="Release v$NEW_VERSION [Ref: $GIT_HASH]"
git commit -m "$COMMIT_MSG"
git tag -a "v$NEW_VERSION" -m "$COMMIT_MSG"
git push origin main
git push origin "v$NEW_VERSION"
echo "✅ Pushed to GitHub. (This will trigger the Actions tab!)"

# 8. DOCKER PUSH (Direct to DigitalOcean)
echo "🐳 Building & Pushing to DigitalOcean..."
docker build --platform linux/amd64 \
  -t $REGISTRY_URL/$IMAGE_NAME:$NEW_VERSION \
  -t $REGISTRY_URL/$IMAGE_NAME:latest \
  .
docker push $REGISTRY_URL/$IMAGE_NAME:$NEW_VERSION
docker push $REGISTRY_URL/$IMAGE_NAME:latest

echo "----------------------------------------------------"
echo "🎉 DONE! v$NEW_VERSION is live."
echo "----------------------------------------------------"