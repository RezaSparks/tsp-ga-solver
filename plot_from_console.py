import matplotlib.pyplot as plt
import sys

def parse_route_from_file(filename):
    coords = []
    # لیست کدگذاری‌های رایج در ویندوز
    encodings = ['utf-8', 'utf-16', 'cp1252', 'latin-1']
    lines = None
    for enc in encodings:
        try:
            with open(filename, 'r', encoding=enc) as f:
                lines = f.readlines()
                break
        except UnicodeDecodeError:
            continue
    # اگر هیچکدام کار نکرد، به‌صورت باینری بخوانیم
    if lines is None:
        with open(filename, 'rb') as f:
            raw = f.read()
            # امتحان UTF-16 و سپس latin-1
            try:
                text = raw.decode('utf-16')
                lines = text.splitlines()
            except:
                text = raw.decode('latin-1')
                lines = text.splitlines()
    if not lines:
        print("خطا در خواندن فایل.")
        return []
    
    for line in lines:
        if 'city' in line and '(' in line and ')' in line:
            start = line.find('(')
            end = line.find(')')
            if start != -1 and end != -1:
                coord_str = line[start+1:end].strip()
                parts = coord_str.split(',')
                if len(parts) == 2:
                    try:
                        x = float(parts[0].strip())
                        y = float(parts[1].strip())
                        coords.append((x, y))
                    except ValueError:
                        continue
    return coords

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python plot_from_console.py output.txt")
        sys.exit(1)
    coords = parse_route_from_file(sys.argv[1])
    if not coords:
        print("هیچ مختصاتی در خروجی پیدا نشد.")
        sys.exit(1)
    # بستن مسیر (برگشت به نقطه اول)
    coords.append(coords[0])
    xs, ys = zip(*coords)
    plt.figure(figsize=(10, 8))
    plt.plot(xs, ys, 'o-', color='green', markersize=8, linewidth=2)
    for i, (x, y) in enumerate(coords[:-1]):
        plt.text(x+5, y+5, str(i), fontsize=9, color='blue')
    plt.title("TSP Best Route")
    plt.xlabel("X Coordinate")
    plt.ylabel("Y Coordinate")
    plt.grid(True)
    plt.show()