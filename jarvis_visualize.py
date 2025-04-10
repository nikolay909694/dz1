import matplotlib.pyplot as plt
import re

def parse_test_file(filename):
    tests = []
    with open(filename, 'r') as f:
        content = f.read().split('---\n')
    
    for test in content:
        if not test.strip():
            continue
            
        points = []
        hull = []
        
        # Parse points
        points_match = re.search(r'POINTS:((?:\s-?\d+,-?\d+)+)', test)
        if points_match:
            points_str = points_match.group(1).strip().split()
            points = [tuple(map(int, p.split(','))) for p in points_str]
        
        # Parse convex hull
        hull_match = re.search(r'HULL:((?:\s-?\d+,-?\d+)+)', test)
        if hull_match:
            hull_str = hull_match.group(1).strip().split()
            hull = [tuple(map(int, p.split(','))) for p in hull_str]
        
        if points and hull:
            tests.append({'points': points, 'hull': hull})
    
    return tests

def visualize_test(test, index):
    fig, ax = plt.subplots(figsize=(10, 8))
    
    # Plot all points
    x_pts = [p[0] for p in test['points']]
    y_pts = [p[1] for p in test['points']]
    ax.scatter(x_pts, y_pts, color='blue', label='Points')
    
    # Plot convex hull
    if test['hull']:
        hull = test['hull']
        hull.append(hull[0])  # Close the hull
        x_hull = [p[0] for p in hull]
        y_hull = [p[1] for p in hull]
        ax.plot(x_hull, y_hull, 'r-', linewidth=2, label='Jarvis Hull')
        ax.fill(x_hull, y_hull, alpha=0.2, color='red')
    
    # Customize plot
    ax.set_title(f'Jarvis March Test Case #{index + 1}')
    ax.set_xlabel('X coordinate')
    ax.set_ylabel('Y coordinate')
    ax.grid(True, linestyle='--', alpha=0.7)
    ax.legend()
    ax.axis('equal')
    
    plt.tight_layout()
    plt.savefig(f'jarvis_test_{index + 1}.png', dpi=100)
    plt.close()

def main():
    tests = parse_test_file('jarvis_tests.log')
    
    print(f"Found {len(tests)} test cases")
    for i, test in enumerate(tests):
        visualize_test(test, i)
        print(f"Generated visualization for test case {i + 1}")
    
    print("\nAll visualizations saved as PNG files")

if __name__ == "__main__":
    main()