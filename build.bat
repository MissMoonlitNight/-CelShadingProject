@echo off


:: 1. Компиляция 
cl /MDd /EHsc /std:c++17 /I dependencies\include /I src CelShaiding\main.cpp /D GLEW_STATIC /link /LIBPATH:dependencies\lib glew32s.lib glfw3.lib assimp-vc143-mtd.lib opengl32.lib user32.lib gdi32.lib shell32.lib comdlg32.lib /NODEFAULTLIB:LIBCMT /NODEFAULTLIB:MSVCRT /OUT:MyApp.exe

if %errorlevel% neq 0 (
    echo BUILD FAILED!
    pause
    exit /b %errorlevel%
)

:: 2. Копирование DLL 
copy /Y "dependencies\lib\assimp-vc143-mtd.dll" "assimp-vc143-mtd.dll"

echo BUILD SUCCESSFUL!
pause