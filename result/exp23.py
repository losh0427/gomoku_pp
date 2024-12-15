import matplotlib.pyplot as plt

# Data
threads = [-2, -4, -8, -16]
win_rates = [50.00, 30.00, 10.00, 40.00]  # Black win rates
average_moves = [11.60, 15.67, 12.00, 8.50]  # Average moves when Black wins



# Corrected thread numbers
threads_corrected = [2, 4, 8, 16]

# Plot 1: Win rate vs. Threads (Corrected)
plt.figure(figsize=(10, 6))
plt.plot(threads_corrected, win_rates, marker='o', label="Win Rate (%)")
plt.title("Black Win Rate vs Threads (Corrected)")
plt.xlabel("Thread Number")
plt.ylabel("Win Rate (%)")
plt.grid(True)
plt.legend()
plt.show()

# Plot 2: Average moves vs Threads (Corrected)
plt.figure(figsize=(10, 6))
plt.plot(threads_corrected, average_moves, marker='o', color='orange', label="Average Moves")
plt.title("Average Moves When Black Wins vs Threads (Corrected)")
plt.xlabel("Thread Number")
plt.ylabel("Average Moves")
plt.grid(True)
plt.legend()
plt.show()
