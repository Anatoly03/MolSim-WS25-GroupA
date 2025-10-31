#!/usr/bin/env bash

# Build CMake
mkdir build
cmake . -B build -DENABLE_VTK_OUTPUT=OFF -Wno-dev

# Build project
cmake --build build -- -j 4

# Run help command to verify build
./build/MolSim -h
