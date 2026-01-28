

# Clean previous build directory if it exists
# if [ -d "build" ]; then rm -rf build; fi
mkdir build

# Build CMake
export CMAKE_POLICY_VERSION_MINIMUM=3.5
<<<<<<< HEAD
cmake -S . -B build -DENABLE_TRACY=OFF -DENABLE_VTK_OUTPUT=OFF -DCMAKE_CXX_STANDARD=17 -DCMAKE_CXX_STANDARD_REQUIRED=ON -Wno-dev
=======
cmake -S . -B build -DENABLE_TRACY=ON -DENABLE_VTK_OUTPUT=OFF -DCMAKE_CXX_STANDARD=17 -DCMAKE_CXX_STANDARD_REQUIRED=ON -Wno-dev
>>>>>>> 04b44ee (cmake add openmp support)

# Build project
make -C build -j4

# Run help command to verify build
./build/MolSim -h short
