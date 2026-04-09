@echo off
mkdir dist
cd dist
mkdir mgb-uml
cd mgb-uml
mkdir icons

:: --- 1. Copy Assets ---
copy ..\..\tikzfiles.reg .
copy ..\..\images\tikzit.ico icons\

:: --- 2. Copy Executable (From the build folder) ---
if exist ..\..\build\release\mgb-uml.exe (
    copy ..\..\build\release\mgb-uml.exe .
) else if exist ..\..\build\mgb-uml.exe (
    copy ..\..\build\mgb-uml.exe .
) else (
    echo "ERROR: Could not find mgb-uml.exe"
    exit /b 1
)

:: --- NEW STEP: INJECT CUSTOM PLUGINS ---
echo Copying MGB-UML Custom Plugins...
if exist ..\..\build\plugins (
    xcopy /E /I /Y ..\..\build\plugins plugins
) else if exist ..\..\build\release\plugins (
    xcopy /E /I /Y ..\..\build\release\plugins plugins
) else (
    echo WARNING: Plugins folder not found!
)

:: Clean up the Windows compiler scratchpad files!
if exist plugins\*.lib del plugins\*.lib
if exist plugins\*.exp del plugins\*.exp

:: --- 3. Copy Poppler DLLs ---
if exist ..\..\poppler-21.11.0\Library\bin (
    copy ..\..\poppler-21.11.0\Library\bin\*.dll .
) else (
    echo "WARNING: Poppler library not found in root folder."
)

:: --- 4. Run Qt Deployment Tool ---
windeployqt.exe --release --compiler-runtime --no-translations --no-opengl-sw --no-system-d3d-compiler .\mgb-uml.exe

:: --- 5. Zip it up ---
cd ..
7z a -tzip mgb-uml-windows.zip mgb-uml
cd ..