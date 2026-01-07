# Performance Testing Guide (Task 4)

## Local Test (Windows)
```bash
cmake --build build --config Release -- -j 4
./build/MolSim.exe input/full_nl_250_50.txt -t 1.0 -d 0.0005 -g -12.44 -N 1000 --Lx 303 --Ly 180 -s 1.2 -e 1.0 -r 3.0 -i
```

## Cluster Test (Linux)
```bash
scp -r MolSim-WS25-GroupA/ <user>@login.coolmuc.de:~
ssh <user>@login.coolmuc.de
cd MolSim-WS25-GroupA
cmake -Bbuild -DCMAKE_BUILD_TYPE=Release && cmake --build build -- -j4
sbatch scripts/run_cluster.sh
cat performance_*.out
```

## Profiling
```bash
bash scripts/profile_gprof.sh
head -n 50 profiling_report.txt
```

## Metrics

**MUPS** = (Particles × Iterations) / Runtime  
Example: 12500 × 2000 / 10s = 2.5 MUPS

## Expected Performance
- Naive O(N²): 0.8-2.5 MUPS
- With neighbor lists: 12-50 MUPS
- + OpenMP (4 cores): 50-125 MUPS
