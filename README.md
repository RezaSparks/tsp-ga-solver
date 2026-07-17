# TSP Genetic Algorithm Solver

[![Build Status](https://github.com/RezaSparks/tsp-ga-solver/actions/workflows/build.yml/badge.svg)](https://github.com/RezaSparks/tsp-ga-solver/actions)

A cross-platform C++17 implementation of a Genetic Algorithm to solve the Traveling Salesman Problem (TSP) with real-time visualization using Raylib.

## Features
- ✅ Real-time visualization of the evolving route
- ✅ Modular C++ architecture (GA, TSP core, Renderer, CLI)
- ✅ Tournament selection + Ordered Crossover (OX)
- ✅ Elitism to preserve the best solutions
- ✅ Configurable population size, generations, and mutation rate via CLI flags
- ✅ Headless mode (`--headless`) for running without a display — CI, SSH, batch runs
- ✅ Reproducible runs via `--seed`
- ✅ Cross-platform CMake build (Linux, macOS, Windows — no Visual Studio required)
- ✅ Unit tests with GoogleTest (distance, crossover validity, elitism property)

## Demo
![TSP GA Solver Demo](screenshot.png)

## How to Build

Requires CMake 3.16+ and a C++17 compiler (GCC, Clang, or MSVC). Raylib is downloaded and built automatically — no manual install needed.

```bash
git clone https://github.com/RezaSparks/tsp-ga-solver.git
cd tsp-ga-solver
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

The executable is written to `build/tsp_solver` (or `build/Release/tsp_solver.exe` on Windows with the Visual Studio generator).

On Linux, you'll also need the X11/OpenGL dev headers Raylib builds against:
```bash
sudo apt-get install libgl1-mesa-dev libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev
```

### Building with Tests

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release -DTSP_BUILD_TESTS=ON
cmake --build build --config Release
ctest --test-dir build --output-on-failure
```

## How to Use

### Basic Run (with visualization)
```bash
./tsp_solver --cities 30 --population 100 --generations 500 --mutation-rate 0.02
```

### Headless Run (no window)
```bash
./tsp_solver --headless --cities 30 --population 100 --generations 500 --mutation-rate 0.02
```

### Reproducible Run (same result every time)
```bash
./tsp_solver --headless --cities 10 --population 50 --generations 200 --seed 42
```

### All CLI Flags
```bash
./tsp_solver --help
```

| Flag | Default | Description |
|------|---------|-------------|
| `--cities, -n` | 20 | Number of cities to generate |
| `--population, -p` | 100 | Population size |
| `--generations, -g` | 500 | Number of generations |
| `--mutation-rate, -m` | 0.02 | Mutation probability (0.0–1.0) |
| `--seed` | 0 | Random seed (0 = auto, any other = fixed) |
| `--headless` | false | Run without Raylib visualization |
| `--help, -h` | — | Print usage and exit |

At the end of a run, the solver prints the best distance found and the full best route (visiting order + coordinates).

## Project Structure
- `/src` — Source files, including the main entry point
- `/include/ga` — Genetic Algorithm core (selection, crossover, mutation, elitism)
- `/include/tsp` — City and distance logic
- `/include/visualization` — Raylib rendering
- `/include/cli` — CLI argument parsing
- `/examples` — Sample input files and ready-to-run commands
- `/tests` — Unit tests (GoogleTest)

## Tests

The test suite covers three critical properties of the solver:

| Test Suite | What It Checks |
|------------|----------------|
| `DistanceTest` | Euclidean distance correctness, symmetry, zero distance for identical points |
| `CrossoverValidity` | Ordered Crossover always produces valid permutations (no duplicates, no missing cities) |
| `ElitismProperty` | Best fitness never worsens across generations, even under high mutation rates |

Run tests:
```bash
cmake -B build -DTSP_BUILD_TESTS=ON
cmake --build build
./build/tests/tsp_tests
```

## Roadmap

- [x] Fix LICENSE
- [x] Replace `std::rand()` with modern `<random>`
- [x] Add `--seed` for reproducible runs
- [x] Add `--headless` mode
- [x] Refactor renderer API
- [x] Add unit tests with GoogleTest
- [ ] Load cities from CSV/TSPLIB files
- [ ] Save results / export final route to file
- [ ] Support for TSPLIB standard benchmarks
- [ ] Additional crossover operators (PMX, Cycle)
- [ ] Additional mutation operators (inversion, scramble)
- [ ] 2-opt local search post-processing
- [ ] Convergence plots and benchmark tables
- [ ] Live web demo (Emscripten)

## License

MIT License — see [LICENSE](LICENSE) for details.