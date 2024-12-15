import numpy as np
import matplotlib.pyplot as plt

# Load data from provided files
file_paths = {

    "Sequential": "./exp1.1/c.txt",
    "Parallel 2 Threads": "./exp1.2/p1_2.txt",
    "Parallel 4 Threads": "./exp1.2/p1_4.txt",
    "Parallel 8 Threads": "./exp1.2/p1_8.txt",
    "Parallel 16 Threads": "./exp1.2/p1_16.txt"

}

# Initialize results
results = {}

# Process each file
for label, path in file_paths.items():
    with open(path, 'r') as file:
        # Read numbers, assuming they are space-separated
        data = list(map(int, file.read().split()))
        # Calculate statistics
        mean = np.mean(data)
        std = np.std(data)
        min_val = np.min(data)
        max_val = np.max(data)
        results[label] = {"mean": mean, "std": std, "min": min_val, "max": max_val}

# Prepare data for plotting
labels = list(results.keys())
means = [results[label]["mean"] for label in labels]
std_devs = [results[label]["std"] for label in labels]
min_vals = [results[label]["min"] for label in labels]
max_vals = [results[label]["max"] for label in labels]

# Plot the results
plt.figure(figsize=(10, 6))
x_pos = np.arange(len(labels))

# Bar plot with error bars
plt.bar(x_pos, means, yerr=std_devs, capsize=5, label="Mean ± Std Dev")
plt.scatter(x_pos, min_vals, color='red', label="Minimum", zorder=5)
plt.scatter(x_pos, max_vals, color='green', label="Maximum", zorder=5)

# Formatting the plot
plt.xticks(x_pos, labels)
plt.xlabel("Execution Type")
plt.ylabel("Iterations in 2 Seconds")
plt.title("MCTS Iterations: Sequential vs Parallel (2-16 Threads)")
plt.legend()
plt.grid(axis='y', linestyle='--', alpha=0.7)

# Show the plot
plt.tight_layout()
plt.show()
