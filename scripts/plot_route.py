#!/usr/bin/env python3
"""Plot TSP route from CSV output."""
import sys
import csv
from pathlib import Path
import matplotlib.pyplot as plt


def load_route(path):
    cities = []
    with open(path) as f:
        reader = csv.DictReader(f)
        headers = [h.strip().lower() for h in (reader.fieldnames or [])]
        for i, row in enumerate(reader):
            city = {}
            # تشخیص ستون index
            if 'index' in headers:
                city['index'] = int(row['index'])
            else:
                city['index'] = i 


            if 'x' in headers:
                city['x'] = float(row['x'])
            elif 'X' in (reader.fieldnames or []):
                city['x'] = float(row['X'])
            else:
                raise KeyError(f"No x column. Headers: {headers}")
            
            # تشخیص ستون y
            if 'y' in headers:
                city['y'] = float(row['y'])
            elif 'Y' in (reader.fieldnames or []):
                city['y'] = float(row['Y'])
            else:
                raise KeyError(f"No y column. Headers: {headers}")
            
            cities.append(city)
    return cities


def plot_route(route_csv, params_file=None, save_path=None):
    cities = load_route(route_csv)
    xs = [c['x'] for c in cities]
    ys = [c['y'] for c in cities]
    indices = [c['index'] for c in cities[:-1]] 

    xs_closed = xs + [xs[0]]
    ys_closed = ys + [ys[0]]

    fig, ax = plt.subplots(figsize=(12, 10))

    ax.plot(xs_closed, ys_closed, 'b-', linewidth=1.5, alpha=0.7, label='Route')
    ax.scatter(xs[:-1], ys[:-1], c='#2196F3', s=100, zorder=5, edgecolors='white', linewidth=2)
    for c in cities[:-1]:
        ax.annotate(str(c['index']), (c['x'], c['y']),
                   textcoords="offset points", xytext=(5, 5),
                   fontsize=8, alpha=0.7)

    ax.scatter([xs[0]], [ys[0]], c='red', s=200, zorder=6, marker='*',
               edgecolors='white', linewidth=2, label='Start')

    title = "TSP Best Route"
    if params_file and Path(params_file).exists():
        with open(params_file, 'r') as f:
            params = dict(line.strip().split(': ', 1) for line in f if ': ' in line)
        title = f"TSP GA Solver — {params.get('Cities', '?')} cities\n"
        title += f"Best distance: {params.get('Best Distance', '?')} | "
        title += f"Crossover: {params.get('Crossover', '?')} | Mutation: {params.get('Mutation', '?')}"

    ax.set_title(title, fontsize=14, fontweight='bold')
    ax.set_xlabel('X Coordinate')
    ax.set_ylabel('Y Coordinate')
    ax.set_aspect('equal')
    ax.grid(True, alpha=0.3)
    ax.legend()

    plt.tight_layout()

    if save_path:
        plt.savefig(save_path, dpi=300, bbox_inches='tight')
        print(f"Saved to: {save_path}")
    else:
        plt.show()


if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("Usage: python plot_route.py <output_dir/>")
        print("   or: python plot_route.py <best_route.csv>")
        sys.exit(1)

    path = Path(sys.argv[1])
    if path.is_dir():
        route_csv = path / 'best_route.csv'
        params = path / 'params.txt'
        save = path / 'route.png'
    else:
        route_csv = path
        params = None
        save = path.with_suffix('.png')

    plot_route(route_csv, params, save)