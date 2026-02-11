#!/bin/bash

# 1. Download the packaging tools (LinuxDeploy)
wget -N https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage
wget -N https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage
chmod +x linuxdeploy*.AppImage

# 2. Set Environment Variables
export QMAKE=/usr/bin/qmake6

# 3. Generate the AppImage
# --- FIX: Changed 'build/tikzit' to 'build/mgb-uml' ---
./linuxdeploy-x86_64.AppImage \
    --appdir AppDir \
    --executable build/mgb-uml \
    --icon-file images/tikzit.svg \
    --desktop-file share/applications/tikzit.desktop \
    --plugin qt \
    --output appimage

# 4. Rename for consistency
# --- FIX: Renaming the output file to mgb-uml ---
mv MGB-UML*.AppImage mgb-uml-linux.AppImage