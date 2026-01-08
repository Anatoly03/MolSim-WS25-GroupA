@echo off
REM Script to build and run the drop simulation on Windows

echo ============================================
echo Drop Simulation Build and Run Script
echo ============================================

REM Create build directory if it doesn't exist
if not exist "build" (
    echo Creating build directory...
    mkdir build
)

cd build

REM Configure with CMake
echo.
echo Configuring with CMake...
cmake .. || (
    echo CMake configuration failed!
    cd ..
    exit /b 1
)

REM Build
echo.
echo Building project...
cmake --build . --config Release || (
    echo Build failed!
    cd ..
    exit /b 1
)

cd ..

REM Create output directory if it doesn't exist
if not exist "output" (
    echo.
    echo Creating output directory...
    mkdir output
)

echo.
echo ============================================
echo Build successful!
echo ============================================
echo.
echo To run the simulations:
echo.
echo 1. Equilibration phase (creates checkpoint):
echo    build\Release\MolSim.exe input\drop-equilibration.yml
echo    OR
echo    build\MolSim.exe input\drop-equilibration.yml
echo.
echo 2. Main simulation (uses checkpoint + adds drop):
echo    build\Release\MolSim.exe input\drop-simulation.yml
echo    OR
echo    build\MolSim.exe input\drop-simulation.yml
echo.
echo 3. Optional: Periodic boundary version:
echo    build\Release\MolSim.exe input\drop-simulation-periodic.yml
echo    OR
echo    build\MolSim.exe input\drop-simulation-periodic.yml
echo.
echo ============================================
