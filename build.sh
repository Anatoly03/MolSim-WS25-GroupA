#!/usr/bin/env bash

# Clean previous build directory if it exists
# if [ -d "build" ]; then rm -rf build; fi
mkdir build

# Build CMake
cmake -S . -B build -DENABLE_VTK_OUTPUT=OFF -DCMAKE_CXX_STANDARD=17 -DCMAKE_CXX_STANDARD_REQUIRED=ON -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -Wno-dev

# Build project
make -C build -j4

# Run help command to verify build
./build/MolSim -h short
