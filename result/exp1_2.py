import numpy as np
import matplotlib.pyplot as plt

# Data from files
p1_data = [
    135262, 144520, 159310, 157462, 167226, 175229, 218068, 
    133412, 150880, 153240, 145176, 160242, 182842, 247225
]
p2_data = [
    122818, 125042, 137097, 143348, 144158, 156276, 169402, 
    116972, 125120, 140068, 138060, 143432, 145623, 179192
]

# Calculate averages
p1_avg = np.mean(p1_data)
p2_avg = np.mean(p2_data)

# Plot bar chart
labels = ['P1 (OpenMP)', 'P2 (Pthread)']
averages = [p1_avg, p2_avg]

plt.bar(labels, averages, color=['blue', 'green'])
plt.title('Average Iterations (Thread Count: 2, Time: 5 seconds)')
plt.ylabel('Average Iterations')
plt.show()
