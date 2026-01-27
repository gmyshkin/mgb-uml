@echo off
mkdir dist
cd dist
mkdir tikzit
cd tikzit
mkdir icons

:: --- 1. Copy Assets ---
copy ..\..\tikzfiles.reg .
copy ..\..\images\tikzit.ico icons\

:: --- 2. Copy Executable (From the build folder) ---
:: QMake puts the exe in build\release by default
if exist ..\..\build\release\tikzit.exe (
    copy ..\..\build\release\tikzit.exe .
) else (
    echo "ERROR: Could not find tikzit.exe in build\release"
    exit /b 1
)

:: --- 3. Copy Poppler DLLs (From the downloaded folder) ---
:: We assume poppler-21.11.0 is in the project root
if exist ..\..\poppler-21.11.0\Library\bin (
    copy ..\..\poppler-21.11.0\Library\bin\*.dll .
) else (
    echo "WARNING: Poppler library not found in root folder."
)

:: --- 4. Run Qt Deployment Tool ---
:: Updated flags for Qt 6 (removed deprecated webkit/angle flags)
windeployqt.exe --release --compiler-runtime --no-translations --no-opengl-sw --no-system-d3d-compiler .\tikzit.exe

:: --- 5. Zip it up ---
cd ..
7z a -tzip tikzit-windows.zip tikzit
cd ..