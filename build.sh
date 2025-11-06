#!/usr/bin/env bash

# Clean previous build directory if it exists
# if [ -d "build" ]; then rm -rf build; fi
mkdir build

# Build CMake
cmake -S . -B build -DENABLE_VTK_OUTPUT=OFF -Wno-dev

# Build project
make -C build -j4

# Run help command to verify build
./build/MolSim -h short
