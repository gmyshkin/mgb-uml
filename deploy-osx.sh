#!/bin/bash

# Ensure we are in the project root
# --- FIX: Changed 'tikzit.app' to 'mgb-uml.app' ---
if [ -d "build/mgb-uml.app" ]; then
    mv build/mgb-uml.app .
fi

if [ ! -d "mgb-uml.app" ]; then
    echo "Error: mgb-uml.app not found in current directory."
    exit 1
fi

# Run the standard Mac deployment tool
# --- FIX: Deploying on mgb-uml.app ---
macdeployqt mgb-uml.app -dmg -verbose=1

# Rename the DMG so the upload step finds it easily
mv mgb-uml.dmg mgb-uml-macos.dmg