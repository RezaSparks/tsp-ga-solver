# Contributing to tsp-ga-solver

Thank you for your interest in contributing! This project follows standard open-source practices. Please read this guide before submitting issues or pull requests.

## Table of Contents
- [Code of Conduct](#code-of-conduct)
- [Getting Started](#getting-started)
- [Development Workflow](#development-workflow)
- [Code Style](#code-style)
- [Testing Requirements](#testing-requirements)
- [Commit Message Guidelines](#commit-message-guidelines)
- [Release Process](#release-process)

## Code of Conduct

Be respectful, constructive, and inclusive. Disagreement is fine; hostility is not.

## Getting Started

### Prerequisites
- CMake 3.16+
- C++17 compiler (GCC 10+, Clang 12+, or MSVC 2019+)
- Git

### Building

```bash
git clone https://github.com/RezaSparks/tsp-ga-solver.git
cd tsp-ga-solver
cmake -B build -DCMAKE_BUILD_TYPE=Release -DTSP_BUILD_TESTS=ON
cmake --build build --config Release
```

### Running Tests

```bash
ctest --test-dir build --output-on-failure
# Or run directly:
./build/tests/tsp_tests
```

All tests must pass before a PR is merged.

## Development Workflow

1. **Fork** the repository
2. **Create a branch** with a descriptive name:
   - `feat/pmx-crossover`
   - `fix/renderer-scaling`
   - `docs/benchmark-table`
3. **Write code** following the style guide below
4. **Add tests** for any new functionality
5. **Ensure CI passes** on all 3 platforms (Linux, macOS, Windows)
6. **Update README** if adding CLI flags or changing behavior
7. **Submit a PR** with a clear description

## Code Style

### Formatting
- Use the provided `.clang-format` (if present) or follow existing style
- Indent with 4 spaces
- Maximum line length: 100 characters
- Braces on the same line (K&R style)

### Naming Conventions
- `snake_case` for functions and variables
- `PascalCase` for structs and classes
- `SCREAMING_SNAKE_CASE` for constants
- `tsp::namespace` for modules

### Architecture Rules
- **GA logic** goes in `tsp_core` (no Raylib dependency)
- **Visualization** goes in `tsp_visualization` (Raylib only)
- **CLI parsing** goes in `tsp_cli` (cxxopts only)
- **Main executable** ties them together
- Never include `<raylib.h>` in `tsp_core` or `tsp_cli`

## Testing Requirements

Every new feature must have tests:

| Feature Type | Required Tests |
|--------------|----------------|
| New crossover | Validity test (produces valid permutation) × 20 seeds |
| New mutation | Validity test (produces valid permutation) × 20 seeds |
| New distance metric | Correctness, symmetry, known-value tests |
| New CLI flag | Parse test (valid/invalid values) |
| New file parser | Valid file, malformed file, missing file tests |
| Bug fix | Regression test that fails before fix, passes after |

Tests go in `tests/test_core.cpp` or a new `tests/test_<feature>.cpp`.

## Commit Message Guidelines

Use [Conventional Commits](https://www.conventionalcommits.org/):

```
<type>(<scope>): <description>

<body>

<footer>
```

Types:
- `feat`: New feature
- `fix`: Bug fix
- `docs`: Documentation changes
- `test`: Test additions/changes
- `refactor`: Code restructuring without behavior change
- `perf`: Performance improvement
- `ci`: CI/CD changes

Examples:
```
feat(ga): add edge recombination crossover

Implements EXX crossover operator for TSP. Adds --crossover exx flag.
Includes validity tests for 20 random seeds.

Closes #18
```

```
fix(renderer): scale TSPLIB coordinates to window

Previously, TSPLIB instances with coordinates > 900 rendered off-screen.
Now computes bounding box and scales to fit with 20px padding.

Fixes #25
```

## Release Process

Releases are managed by maintainers:

1. Update `CMakeLists.txt` version
2. Update `CHANGELOG.md` (if present)
3. Tag: `git tag -a vX.Y.Z -m "Release vX.Y.Z"`
4. Push tag: `git push origin vX.Y.Z`
5. GitHub Actions builds and attaches binaries automatically
6. Draft release notes on GitHub

## Questions?

Open a [Discussion](https://github.com/RezaSparks/tsp-ga-solver/discussions) or reach out via the issue tracker.
