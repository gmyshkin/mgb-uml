#!/bin/bash

# 1. Download the packaging tools (LinuxDeploy)
wget -N https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage
wget -N https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage
chmod +x linuxdeploy*.AppImage

# 2. Set Environment Variables
# Tell linuxdeploy where to find QMake so it can discover plugins
export QMAKE=/usr/bin/qmake6

# 3. Generate the AppImage
# We assume the binary is in build/tikzit (from the qmake build)
./linuxdeploy-x86_64.AppImage \
    --appdir AppDir \
    --executable build/tikzit \
    --icon-file images/tikzit.svg \
    --desktop-file share/applications/tikzit.desktop \
    --plugin qt \
    --output appimage

# 4. Rename for consistency
mv TikZiT*.AppImage tikzit-linux.AppImage