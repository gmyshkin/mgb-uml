#!/bin/bash

# 1. Download the packaging tools (LinuxDeploy)
wget -N https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage
wget -N https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage
chmod +x linuxdeploy*.AppImage

# 2. Set Environment Variables
export QMAKE=/usr/bin/qmake6

# 3. Create a temporary MGB-UML desktop file
cp share/applications/tikzit.desktop mgb-uml.desktop
sed -i 's/Exec=tikzit/Exec=mgb-uml/g' mgb-uml.desktop
sed -i 's/Name=TikZiT/Name=MGB-UML/g' mgb-uml.desktop

# --- NEW STEP: INJECT CUSTOM PLUGINS ---
echo "Injecting MGB-UML Plugins into AppDir..."
mkdir -p AppDir/usr/bin/plugins
if [ -d "build/plugins" ]; then
    cp build/plugins/* AppDir/usr/bin/plugins/
fi

# 4. Generate the AppImage
./linuxdeploy-x86_64.AppImage \
    --appdir AppDir \
    --executable build/mgb-uml \
    --icon-file images/tikzit.svg \
    --desktop-file mgb-uml.desktop \
    --plugin qt \
    --output appimage

# 5. Rename for consistency
mv MGB-UML*.AppImage mgb-uml-linux.AppImage