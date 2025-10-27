#!/usr/bin/env bash

# Build CMake
cmake . -DENABLE_VTK_OUTPUT=OFF

# Build project
make -j 4 

# Run help command to verify build
./MolSim -h
