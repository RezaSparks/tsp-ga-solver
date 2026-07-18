# Example Inputs / Sample Runs

## Sample TSPLIB files

| File | Cities | Optimal | Notes |
|------|--------|---------|-------|
| `berlin52.tsp` | 52 | 7542 | Classic TSPLIB instance, EUC_2D |

Load and solve:
```bash
./tsp_solver --tsplib examples/berlin52.tsp --population 200 --generations 2000 --headless
```

## Sample CSV files

| File | Cities | Notes |
|------|--------|-------|
| `cities_5.csv` | 5 | A square with a center point — easy to eyeball optimal |
| `cities_10.csv` | 10 | Randomly generated, fixed seed (42) |
| `cities_20.csv` | 20 | Randomly generated, fixed seed (7) |

Load and solve:
```bash
./tsp_solver --csv examples/cities_10.csv --population 100 --generations 500 --headless
```

## Quick smoke tests

Random cities, headless:
```bash
./tsp_solver --headless --cities 5 --population 30 --generations 100
```

Reproducible run:
```bash
./tsp_solver --headless --cities 10 --population 50 --generations 200 --seed 42
```

Multi-run benchmark:
```bash
./tsp_solver --headless --cities 20 --population 100 --generations 300 --runs 10 --seed 7
```

Export convergence data:
```bash
./tsp_solver --headless --cities 30 --population 150 --generations 1000 --output-csv results.csv
```

Try different operators:
```bash
./tsp_solver --headless --cities 20 --crossover pmx --mutation inversion --generations 500 --seed 42
```

With visualization:
```bash
./tsp_solver --cities 50 --population 150 --generations 1000 --mutation-rate 0.03
```

See all flags:
```bash
./tsp_solver --help
```
