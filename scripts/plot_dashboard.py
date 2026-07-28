#!/usr/bin/env python3
import sys
from pathlib import Path
import matplotlib.pyplot as plt
from plot_route import load_route
from plot_convergence import load_convergence

def create_dashboard(output_dir):
    output_dir = Path(output_dir)

    route_csv = output_dir / 'best_route.csv'
    conv_csv = output_dir / 'convergence.csv'
    params_file = output_dir / 'params.txt'

    cities = load_route(route_csv)
    gens, best, avg = load_convergence(conv_csv)

    params = {}
    if params_file.exists():
        with open(params_file, 'r') as f:
            params = dict(line.strip().split(': ', 1) for line in f if ': ' in line)

    fig = plt.figure(figsize=(16, 8))

    ax1 = plt.subplot(1, 2, 1)
    xs = [c['x'] for c in cities]
    ys = [c['y'] for c in cities]

    xs_closed = xs + [xs[0]]
    ys_closed = ys + [ys[0]]

    ax1.plot(xs_closed, ys_closed, 'b-', linewidth=1.5, alpha=0.7)
    ax1.scatter(xs[:-1], ys[:-1], c='#2196F3', s=80, zorder=5, edgecolors='white')
    ax1.scatter([xs[0]], [ys[0]], c='red', s=150, zorder=6, marker='*')
    ax1.set_title('Best Route Found', fontsize=13, fontweight='bold')
    ax1.set_aspect('equal')
    ax1.grid(True, alpha=0.3)


    ax2 = plt.subplot(1, 2, 2)
    ax2.plot(gens, best, 'b-', linewidth=2, label='Best')
    ax2.plot(gens, avg, 'r--', linewidth=1.5, alpha=0.7, label='Average')
    ax2.fill_between(gens, best, avg, alpha=0.1, color='gray')
    ax2.set_xlabel('Generation')
    ax2.set_ylabel('Distance')
    ax2.set_title('Convergence', fontsize=13, fontweight='bold')
    ax2.legend()
    ax2.grid(True, alpha=0.3)
    ax2.set_yscale('log')

    title = f"TSP GA — {params.get('Cities', '?')} cities | "
    title += f"Best: {params.get('Best Distance', '?')}"
    fig.suptitle(title, fontsize=16, fontweight='bold', y=0.98)

    plt.tight_layout(rect=[0, 0, 1, 0.95])

    save_path = output_dir / 'dashboard.png'
    plt.savefig(save_path, dpi=300, bbox_inches='tight')
    print(f"Dashboard saved to: {save_path}")
    plt.show()

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("Usage: python plot_dashboard.py <output_dir/>")
        sys.exit(1)

    create_dashboard(sys.argv[1])