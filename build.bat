@echo off
cd /d %~dp0
cd install
mkdir cmake-build-msvc
cd cmake-build-msvc
cmake -G"Visual Studio 17 2022" ..\..
cmake --build . --target install