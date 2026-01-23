#!/usr/bin/env python3
"""
Run strong-scaling benchmark for Rayleigh–Taylor 3D input.
Generates bench_results.csv and bench_<strategy>_<threads>.log files.

Usage: ./run_strong_scaling.py
"""
import os
import re
import csv
import time
import subprocess

ROOT = os.path.abspath(os.path.dirname(__file__))
EXE = os.path.join(ROOT, 'build', 'MolSim')
INPUT_YAML = os.path.join('input', 'rayleigh_taylor_3d.yml')
# write outputs into the repository `output/bench` directory
OUT_DIR = os.path.join(ROOT, 'output', 'bench')
os.makedirs(OUT_DIR, exist_ok=True)
OUTCSV = os.path.join(OUT_DIR, 'bench_results.csv')

STRATEGIES = ['buffer', 'critical']
THREADS = [1,2,4,8,14,16,28,56]

def main():
    if not os.path.isfile(EXE):
        print('ERROR: executable not found:', EXE)
        return 1
    if not os.path.isfile(INPUT_YAML):
        print('ERROR: input YAML not found:', INPUT_YAML)
        return 1

    with open(INPUT_YAML, 'r') as f:
        base = f.read()

    # CSV header
    with open(OUTCSV, 'w', newline='') as cf:
        w = csv.writer(cf)
        w.writerow(['strategy','threads','seconds','exit_code','logfile'])

    for strat in STRATEGIES:
        for t in THREADS:
            tmp_path = os.path.join('input', f'tmp_{strat}_{t}.yml')
            new = re.sub(r'parallel_strategy:\s*\w+', f'parallel_strategy: {strat}', base)
            with open(tmp_path, 'w') as tf:
                tf.write(new)

            logfile = os.path.abspath(os.path.join(OUT_DIR, f'bench_{strat}_{t}.log'))
            env = os.environ.copy()
            env['OMP_NUM_THREADS'] = str(t)

            print(f'Running strategy={strat} threads={t} -> {logfile}')
            start = time.perf_counter()
            with open(logfile, 'wb') as lf:
                p = subprocess.Popen([EXE, tmp_path], stdout=lf, stderr=subprocess.STDOUT, env=env)
                p.wait()
            elapsed = time.perf_counter() - start
            print(f'Finished {strat} {t}: {elapsed:.3f}s exit={p.returncode}')

            with open(OUTCSV, 'a', newline='') as cf:
                w = csv.writer(cf)
                w.writerow([strat, t, f'{elapsed:.6f}', p.returncode, logfile])

    print('All runs finished. Results:', os.path.abspath(OUTCSV))
    return 0

if __name__ == '__main__':
    raise SystemExit(main())
