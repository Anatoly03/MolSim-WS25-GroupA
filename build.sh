

# Clean previous build directory if it exists
# if [ -d "build" ]; then rm -rf build; fi
mkdir -p build

# Build CMake
export CMAKE_POLICY_VERSION_MINIMUM=3.5

# OpenMP is optional: set ENABLE_OPENMP=ON/OFF when invoking this script
ENABLE_OPENMP=${ENABLE_OPENMP:-OFF}

cmake -S . -B build \
	-DENABLE_TRACY=OFF \
	-DENABLE_VTK_OUTPUT=OFF \
	-DENABLE_OPENMP=${ENABLE_OPENMP} \
	-DCMAKE_CXX_STANDARD=17 \
	-DCMAKE_CXX_STANDARD_REQUIRED=ON \
	-Wno-dev

# Build project
make -C build -j4

# Run help command to verify build
./build/MolSim -h short
