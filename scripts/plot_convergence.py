#!/usr/bin/env python3
import sys
import csv
import matplotlib.pyplot as plt
from pathlib import Path

def load_convergence(csv_path):
    generations = []
    best = []
    avg = []

    with open(csv_path, 'r') as f:
        reader = csv.DictReader(f)
        for row in reader:
            generations.append(int(row['generation']))
            best.append(float(row['best_fitness']))
            avg.append(float(row['avg_fitness']))

    return generations, best, avg

def plot_convergence(conv_csv, save_path=None):
    gens, best, avg = load_convergence(conv_csv)

    fig, ax = plt.subplots(figsize=(12, 7))

    ax.plot(gens, best, 'b-', linewidth=2, label='Best Distance')
    ax.plot(gens, avg, 'r--', linewidth=1.5, alpha=0.7, label='Average Distance')
    ax.fill_between(gens, best, avg, alpha=0.1, color='gray')

    ax.set_xlabel('Generation', fontsize=12)
    ax.set_ylabel('Distance', fontsize=12)
    ax.set_title('GA Convergence Over Generations', fontsize=14, fontweight='bold')
    ax.legend(fontsize=11)
    ax.grid(True, alpha=0.3)
    ax.set_yscale('log')

    plt.tight_layout()

    if save_path:
        plt.savefig(save_path, dpi=300, bbox_inches='tight')
        print(f"Saved to: {save_path}")
    else:
        plt.show()

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("Usage: python plot_convergence.py <output_dir/>")
        print("   or: python plot_convergence.py <convergence.csv>")
        sys.exit(1)

    path = Path(sys.argv[1])
    if path.is_dir():
        conv_csv = path / 'convergence.csv'
        save = path / 'convergence.png'
    else:
        conv_csv = path
        save = path.with_suffix('.png')

    plot_convergence(conv_csv, save)