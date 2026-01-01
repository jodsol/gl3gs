@echo off
REM 1. MSVC 환경 불러오기
REM labtop call "C:\Program Files (x86)\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat" -arch=x64
call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat" -arch=x64

REM 2. CMake Configure (Ninja)
cmake -S . -B build -G Ninja

REM 3. CMake Build
cmake --build build --config Debug
