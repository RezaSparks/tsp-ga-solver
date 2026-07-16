# TSP Genetic Algorithm Solver

[![Build Status](https://github.com/RezaSparks/tsp-genetic-algorithm-c/actions/workflows/build.yml/badge.svg)](https://github.com/RezaSparks/tsp-genetic-algorithm-c/actions)

A cross-platform implementation of a Genetic Algorithm to solve the Traveling Salesman Problem (TSP) with real-time visualization using Raylib.

## Features
- ✅ Real-time visualization of the evolving route
- ✅ Modular C++ architecture (GA, TSP core, Renderer)
- ✅ Tournament selection + Ordered Crossover (OX)
- ✅ Elitism to preserve the best solutions
- ✅ Configurable population size, generations, and mutation rate

## Demo
*(Insert a screenshot here! Press Print Screen while the app runs, save as `screenshot.png` in the repo, and link it)*
![Demo](screenshot.png)

## How to Build (Visual Studio 2022)
1. Clone the repository.
2. Open `TSP_GA.sln`.
3. Install Raylib via NuGet (Project → Manage NuGet Packages → Browse → `raylib`).
4. Build and run (`F5`).

## How to Use
Run the executable. Enter the number of cities, population size, generations, and mutation rate when prompted. Watch the algorithm find a shorter route in real-time!

## Project Structure
- `/src` - Main entry point
- `/ga` - Genetic Algorithm core (selection, crossover, mutation)
- `/tsp` - City and distance logic
- `/visualization` - Raylib rendering

## Future Improvements
- Command-line argument parsing
- Support for TSPLIB standard benchmarks
- Additional crossover operators (PMX, Cycle)