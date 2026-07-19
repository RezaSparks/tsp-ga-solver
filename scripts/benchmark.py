#!/usr/bin/env python3
# scripts/benchmark.py
# Run TSPLIB benchmarks and generate markdown results table.
# Usage: python3 scripts/benchmark.py [--solver ./build/tsp_solver]

import argparse
import os
import re
import subprocess
import sys
import time
import urllib.request
from pathlib import Path

INSTANCES = {
    "berlin52": {"cities": 52, "optimal": 7542},
    "att48": {"cities": 48, "optimal": 10628},
    "kroA100": {"cities": 100, "optimal": 21282},
}

POPULATION = 200
GENERATIONS = 2000
RUNS = 10


def download_instance(name: str, dest_dir: Path) -> Path:
    """Download TSPLIB instance if not present."""
    dest = dest_dir / f"{name}.tsp"
    if dest.exists():
        return dest

    url = f"http://comopt.ifi.uni-heidelberg.de/software/TSPLIB95/tsp/{name}.tsp.gz"
    gz_path = dest_dir / f"{name}.tsp.gz"

    print(f"Downloading {name}...")
    try:
        urllib.request.urlretrieve(url, gz_path)
        import gzip
        with gzip.open(gz_path, 'rb') as f_in:
            with open(dest, 'wb') as f_out:
                f_out.write(f_in.read())
        gz_path.unlink()
        return dest
    except Exception as e:
        print(f"Error downloading {name}: {e}")
        return None


def run_benchmark(solver: Path, instance: str, tsp_file: Path) -> dict:
    """Run solver and parse statistics."""
    cmd = [
        str(solver),
        "--tsplib", str(tsp_file),
        "--population", str(POPULATION),
        "--generations", str(GENERATIONS),
        "--runs", str(RUNS),
        "--crossover", "pmx",
        "--mutation", "inversion",
        "--headless",
        "--seed", "42",
    ]

    print(f"  Running {instance}...", flush=True)
    start = time.time()
    result = subprocess.run(cmd, capture_output=True, text=True)
    elapsed = time.time() - start

    if result.returncode != 0:
        print(f"  ERROR: {result.stderr}")
        return None

    output = result.stdout

    # Parse aggregate statistics
    stats = {"time": elapsed / RUNS}  # average time per run

    for line in output.splitlines():
        m = re.match(r"Best:\s+([0-9.]+)", line)
        if m:
            stats["best"] = float(m.group(1))
        m = re.match(r"Avg:\s+([0-9.]+)", line)
        if m:
            stats["avg"] = float(m.group(1))
        m = re.match(r"Worst:\s+([0-9.]+)", line)
        if m:
            stats["worst"] = float(m.group(1))

    return stats


def main():
    parser = argparse.ArgumentParser(description="Run TSPLIB benchmarks")
    parser.add_argument("--solver", default="./build/tsp_solver", help="Path to solver binary")
    parser.add_argument("--output", default="-", help="Output file (- for stdout)")
    args = parser.parse_args()

    solver = Path(args.solver)
    if not solver.exists():
        print(f"Error: solver not found at {solver}")
        print("Build first: cmake -B build && cmake --build build")
        sys.exit(1)

    benchmarks_dir = Path("benchmarks")
    benchmarks_dir.mkdir(exist_ok=True)

    print("TSP-GA-Solver Benchmark Results")
    print(f"Population: {POPULATION}, Generations: {GENERATIONS}, Runs: {RUNS}")
    print()

    results = []
    for name, info in INSTANCES.items():
        tsp_file = download_instance(name, benchmarks_dir)
        if not tsp_file:
            continue

        stats = run_benchmark(solver, name, tsp_file)
        if not stats:
            continue

        optimal = info["optimal"]
        gap = ((stats["avg"] - optimal) / optimal) * 100

        results.append({
            "name": name,
            "cities": info["cities"],
            "optimal": optimal,
            "best": stats["best"],
            "avg": stats["avg"],
            "worst": stats["worst"],
            "gap": gap,
            "time": stats["time"],
        })

    # Generate markdown table
    lines = []
    lines.append("| Instance | Cities | Optimal | Best Found | Avg Found | Worst Found | Gap % | Avg Time (s) |")
    lines.append("|----------|--------|---------|------------|-----------|-------------|-------|--------------|")

    for r in results:
        lines.append(
            f"| {r['name']} | {r['cities']} | {r['optimal']} | "
            f"{r['best']:.0f} | {r['avg']:.0f} | {r['worst']:.0f} | "
            f"{r['gap']:.1f} | {r['time']:.1f} |"
        )

    lines.append("")
    lines.append("> **Note:** Gap % = ((avg_found - optimal) / optimal) × 100. Lower is better.")
    lines.append("> A pure GA typically achieves 10–30% gaps. Hybrid approaches (2-opt) can reduce to < 5%.")

    output = "\n".join(lines)

    if args.output == "-":
        print(output)
    else:
        with open(args.output, "w") as f:
            f.write(output)
        print(f"Results written to {args.output}")


if __name__ == "__main__":
    main()
