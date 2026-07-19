
## Benchmarks

The solver has been evaluated on standard TSPLIB instances. All runs use:
- Population: 200
- Generations: 2000
- Crossover: PMX
- Mutation: Inversion
- Independent runs: 10

| Instance | Cities | Optimal | Best Found | Avg Found | Worst Found | Gap % | Avg Time (s) |
|----------|--------|---------|------------|-----------|-------------|-------|--------------|
| berlin52 | 52     | 7542    | —          | —         | —           | —     | —            |
| att48    | 48     | 10628   | —          | —         | —           | —     | —            |

> **Run benchmarks yourself:**
> ```bash
> python3 scripts/benchmark.py --solver ./build/tsp_solver
> ```
> This downloads TSPLIB instances automatically and fills in the table above.

> **Note:** Gap % is calculated as `((avg_found - optimal) / optimal) × 100`.
> A pure genetic algorithm typically achieves 10–30% gaps on these instances.
> Adding 2-opt local search (planned for v1.1.0) can reduce gaps to < 5%.
