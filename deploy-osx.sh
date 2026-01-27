#!/bin/bash

# Ensure we are in the project root
if [ -d "build/tikzit.app" ]; then
    mv build/tikzit.app .
fi

if [ ! -d "tikzit.app" ]; then
    echo "Error: tikzit.app not found in current directory."
    exit 1
fi

# Run the standard Mac deployment tool
# -dmg : Automatically creates the DMG file (simplifies the script)
# -verbose=1 : Shows us what is happening
macdeployqt tikzit.app -dmg -verbose=1

# Rename the DMG so the upload step finds it easily
mv tikzit.dmg tikzit-macos.dmg