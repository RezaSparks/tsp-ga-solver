# Example Inputs / Sample Runs

## ⚠️ Current status

The `.csv` files in this folder show the format city input **will** use once
file loading lands (Milestone 2: "Load data from files" / "Support CSV/TXT
format"). **The solver does not read these files yet** — it always generates
random cities via `--cities N`. They're checked in early so the format is
settled and Milestone 2 has real fixtures to test against.

Until then, use the "Sample runs" below — those work today.

## Sample input files

| File | Cities | Notes |
|---|---|---|
| `cities_5.csv` | 5 | A square with a center point — easy to eyeball the optimal tour by hand. |
| `cities_10.csv` | 10 | Randomly generated, fixed seed (42) for reproducibility. |
| `cities_20.csv` | 20 | Randomly generated, fixed seed (7) for reproducibility. |

Format: one city per line, `x,y`, with a `x,y` header row.

## Sample runs (work today)

Small, fast smoke test:
```bash
./tsp_solver --headless --cities 5 --population 30 --generations 100
```

Reproducible run — same `--seed` always gives the same route, useful for
comparing parameter changes fairly:
```bash
./tsp_solver --headless --cities 10 --population 50 --generations 200 --seed 42
```

Larger run, matching the scale of `cities_20.csv`:
```bash
./tsp_solver --headless --cities 20 --population 100 --generations 300 --mutation-rate 0.05 --seed 7
```

With the visualization window (drop `--headless` to watch it evolve live):
```bash
./tsp_solver --cities 50 --population 150 --generations 1000 --mutation-rate 0.03
```

See all available flags:
```bash
./tsp_solver --help
```
