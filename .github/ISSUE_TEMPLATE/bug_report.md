---
name: Bug report
about: Report a crash, incorrect result, or build failure
title: '[BUG] '
labels: bug
assignees: ''
---

## Description
A clear and concise description of the bug.

## To Reproduce
Steps to reproduce the behavior:
1. Build with `cmake -B build -DTSP_BUILD_TESTS=ON`
2. Run `./build/tsp_solver <your-flags>`
3. See error

## Expected Behavior
What you expected to happen.

## Actual Behavior
What actually happened (include full error output, stack trace, or screenshot).

## Environment
- OS: [e.g. Ubuntu 22.04, macOS 14, Windows 11]
- Compiler: [e.g. GCC 12.3, Clang 16, MSVC 2022]
- CMake version: [e.g. 3.28]
- Commit hash: [e.g. `a1b2c3d`]

## Minimal Reproduction
If possible, provide the smallest command that triggers the bug:
```bash
./tsp_solver --headless --cities 10 --population 20 --generations 50 --seed 42
```

## Additional Context
Add any other context about the problem here.
