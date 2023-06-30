cd "$(dirname "${BASH_SOURCE[0]}")"
cd install
mkdir cmake-build-mingw
cd cmake-build-mingw
cmake -G"MinGW Makefiles" ..\..
cmake --build . --target install