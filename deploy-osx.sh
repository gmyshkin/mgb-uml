#!/bin/bash

# Ensure we are in the project root
if [ -d "build/mgb-uml.app" ]; then
    mv build/mgb-uml.app .
fi

if [ ! -d "mgb-uml.app" ]; then
    echo "Error: mgb-uml.app not found in current directory."
    exit 1
fi

# --- NEW STEP: INJECT CUSTOM PLUGINS ---
echo "Injecting MGB-UML Plugins into Mac Bundle..."
mkdir -p mgb-uml.app/Contents/MacOS/plugins
if [ -d "build/plugins" ]; then
    cp build/plugins/* mgb-uml.app/Contents/MacOS/plugins/
fi

# Run the standard Mac deployment tool
macdeployqt mgb-uml.app -dmg -verbose=1

# Rename the DMG so the upload step finds it easily
mv mgb-uml.dmg mgb-uml-macos.dmg