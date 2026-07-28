import pandas as pd
import matplotlib.pyplot as plt
import sys
import os

def plot_route_from_csv(csv_file):
    """رسم مسیر نهایی از فایل CSV"""
    if not os.path.exists(csv_file):
        print(f"فایل {csv_file} پیدا نشد.")
        return
    
    df = pd.read_csv(csv_file)
    
    if 'x' in df.columns and 'y' in df.columns:
        coords = list(zip(df['x'], df['y']))
        coords.append(coords[0])  # بستن مسیر
        xs, ys = zip(*coords)
        
        plt.figure(figsize=(10, 8))
        plt.plot(xs, ys, 'o-', color='green', markersize=8, linewidth=2)
        for i, (x, y) in enumerate(coords[:-1]):
            plt.text(x+2, y+2, str(i), fontsize=9, color='blue')
        plt.title("Best Route Found by GA")
        plt.xlabel("X Coordinate")
        plt.ylabel("Y Coordinate")
        plt.grid(True)
        plt.show()
    else:
        print("فایل CSV باید شامل ستون‌های 'x' و 'y' باشد.")

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python plot_route.py <csv_file>")
        print("Example: python plot_route.py route.csv")
        sys.exit(1)
    plot_route_from_csv(sys.argv[1])