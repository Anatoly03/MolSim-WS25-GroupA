@echo off
REM Performance measurement script for Windows

echo === MolSim Performance Measurement ===
echo Date: %date% %time%
echo.

if not exist build\MolSim.exe (
    echo ERROR: build\MolSim.exe not found!
    echo Please build with: cmake --build build --config Release
    exit /b 1
)

set INPUT_FILE=input\full_nl_250_50.txt
set T_END=1.0
set DT=0.0005

echo === Test Parameters ===
echo Input: %INPUT_FILE%
echo Time: 0 -^> %T_END% (dt=%DT%)
echo Particles: 12500
echo I/O: disabled
echo.

echo === Running Performance Test (3 runs) ===
echo.

for /L %%i in (1,1,3) do (
    echo.
    echo ===== Run %%i/3 =====
    build\MolSim.exe %INPUT_FILE% ^
        -t %T_END% ^
        -d %DT% ^
        -g -12.44 ^
        --nthermostat 1000 ^
        --Lx 303 ^
        --Ly 180 ^
        --sigma 1.2 ^
        --epsilon 1.0 ^
        --rcut 3.0 ^
        --no-io 2^>nul | findstr /C:"Simulation loop time" /C:"MUPS"
    echo.
)

echo === Performance Test Complete ===
pause
