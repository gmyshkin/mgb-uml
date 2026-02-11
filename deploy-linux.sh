#!/bin/bash

# 1. Download the packaging tools (LinuxDeploy)
wget -N https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage
wget -N https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage
chmod +x linuxdeploy*.AppImage

# 2. Set Environment Variables
export QMAKE=/usr/bin/qmake6

# --- NEW STEP: Create a temporary MGB-UML desktop file ---
# We copy the original file and change "Exec=tikzit" to "Exec=mgb-uml"
cp share/applications/tikzit.desktop mgb-uml.desktop
sed -i 's/Exec=tikzit/Exec=mgb-uml/g' mgb-uml.desktop
sed -i 's/Name=TikZiT/Name=MGB-UML/g' mgb-uml.desktop

# 3. Generate the AppImage
# Note: We now use the NEW 'mgb-uml.desktop' file we just created
./linuxdeploy-x86_64.AppImage \
    --appdir AppDir \
    --executable build/mgb-uml \
    --icon-file images/tikzit.svg \
    --desktop-file mgb-uml.desktop \
    --plugin qt \
    --output appimage

# 4. Rename for consistency
mv MGB-UML*.AppImage mgb-uml-linux.AppImage