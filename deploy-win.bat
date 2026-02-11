@echo off
mkdir dist
cd dist
mkdir mgb-uml
cd mgb-uml
mkdir icons

:: --- 1. Copy Assets ---
:: (Keeping original asset names unless you renamed the actual files on disk)
copy ..\..\tikzfiles.reg .
copy ..\..\images\tikzit.ico icons\

:: --- 2. Copy Executable (From the build folder) ---
:: --- FIX: Looking for mgb-uml.exe ---
if exist ..\..\build\release\mgb-uml.exe (
    copy ..\..\build\release\mgb-uml.exe .
) else (
    echo "ERROR: Could not find mgb-uml.exe in build\release"
    exit /b 1
)

:: --- 3. Copy Poppler DLLs ---
if exist ..\..\poppler-21.11.0\Library\bin (
    copy ..\..\poppler-21.11.0\Library\bin\*.dll .
) else (
    echo "WARNING: Poppler library not found in root folder."
)

:: --- 4. Run Qt Deployment Tool ---
:: --- FIX: Deploying mgb-uml.exe ---
windeployqt.exe --release --compiler-runtime --no-translations --no-opengl-sw --no-system-d3d-compiler .\mgb-uml.exe

:: --- 5. Zip it up ---
cd ..
7z a -tzip mgb-uml-windows.zip mgb-uml
cd ..