import matplotlib.pyplot as plt
from matplotlib.ticker import AutoMinorLocator
import numpy as np

# Data
systems = ['Triton', '2021 MacBook Pro M1 Max', 'FPGA']
fps = [260, 4340, 1300]
latency = [3.84, 0.21, 0.866]

# Color palette
fps_color = "g"
latency_color = "b"

# Sort systems and data based on FPS
sorted_data = sorted(zip(fps, systems, latency))
fps, systems, latency = zip(*sorted_data)

# Create figure and axis objects
fig, ax1 = plt.subplots()

# Twin the axis
ax2 = ax1.twinx()

# Bar width
width = 0.4

# Make the bar plot for FPS (y-axis on the left)
ax1.bar([x - width/2 for x in range(len(systems))], fps, width=width, color=fps_color, label='FPS')
ax1.set_xlabel("")
ax1.set_ylabel("FPS", color=fps_color)
ax1.set_yticks(range(0, 5500, 500))
ax1.yaxis.set_minor_locator(AutoMinorLocator())
ax1.tick_params(axis='y', colors='k')
ax1.set_xticks(range(len(systems)))
ax1.set_xticklabels(systems, color='k')
for label in ax1.get_xticklabels():
    label.set_fontweight('bold')

# Make the bar plot for Latency (y-axis on the right)
ax2.bar([x + width/2 for x in range(len(systems))], latency, width=width, color=latency_color, label='Latency (ms)')
ax2.set_ylabel("Latency (ms)", color=latency_color)
ax2.set_yticks(np.arange(0, 5.5, 0.5))
ax2.yaxis.set_minor_locator(AutoMinorLocator())
ax2.tick_params(axis='y', colors='k')


# Add value labels at the top of each bar
for i, v in enumerate(fps):
    ax1.text(i - width/2, v + 50, str(v), ha='center', va='bottom', color='k')

for i, v in enumerate(latency):
    ax2.text(i + width/2, v + 0.05, str(v), ha='center', va='bottom', color='k')

# Set title
plt.title("FPS and Latency Comparison of Different Systems")

# Add legend
# fig.legend(loc="upper right")

# Show the plot
plt.show()