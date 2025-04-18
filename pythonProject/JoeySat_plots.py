import matplotlib.pyplot as plt
import numpy as np
import matplotlib.ticker as ticker
from JoeySat_GetData import unixtimes_23, arrays_23, unixtimes_24, arrays_24, xlim_23, xlim_24, title_map, color_map, format_time


# Set up the plot
fig, axes = plt.subplots(nrows=4, ncols=2, figsize=(12, 12), sharex=False, gridspec_kw={'width_ratios': [1.5, 1.1]})

# Adjusted order: "All particles" first
plot_order = ["all", "class1", "class2", "class3"]

# Plot each dataset for both days
for i, key in enumerate(plot_order):
    # Left column → 23rd March
    axes[i, 0].plot(unixtimes_23[key], arrays_23[key][:, 3], linestyle='-', marker='o', 
                     markersize=3, linewidth=0.8, label=title_map[key], color=color_map[key])
    axes[i, 0].set_xlim(xlim_23)

    # Right column → 24th March
    axes[i, 1].plot(unixtimes_24[key], arrays_24[key][:, 3], linestyle='-', marker='o', 
                     markersize=3, linewidth=0.8, label=title_map[key], color=color_map[key])
    axes[i, 1].set_xlim(xlim_24)

    # Legend
    axes[i, 0].legend()
    axes[i, 1].legend()

    # Logarithmic Y-axis
    for ax in [axes[i, 0], axes[i, 1]]:
        ax.set_yscale("log")
        ax.set_ylim(2e-2, 1e2)
        ax.grid(True, which='both', linestyle='--', linewidth=0.5)
        ax.set_ylabel("Flux [#/cm2/s]")

# Generate **evenly spaced** x-ticks
num_ticks_23 = 15  # 15 x-ticks for 23rd March
num_ticks_24 = 11  # 11 x-ticks for 24th March

def generate_xticks(start, end, num_ticks):
    timestamps = np.linspace(start, end, num=num_ticks)
    labels = [format_time(ts) for ts in timestamps]
    return timestamps, labels

xticks_23, labels_23 = generate_xticks(xlim_23[0], xlim_23[1], num_ticks_23)
xticks_24, labels_24 = generate_xticks(xlim_24[0], xlim_24[1], num_ticks_24)

# Apply new x-ticks to **ALL plots** to ensure consistent grid
for i in range(4):  # Apply to all subplots
    axes[i, 0].set_xticks(xticks_23)
    axes[i, 0].set_xticklabels(labels_23, rotation=45)

    axes[i, 1].set_xticks(xticks_24)
    axes[i, 1].set_xticklabels(labels_24, rotation=45)

# Remove x-labels for class1, class2, and class3
for ax in axes[:-1, :]:  
    ax[0].set_xticklabels([])
    ax[1].set_xticklabels([])

# Set **identical grid** for all plots
for col in range(2):  # 0 = left column, 1 = right column
    axes[-1, col].yaxis.set_major_locator(ticker.LogLocator(base=10.0, subs=[], numticks=10))
    axes[-1, col].yaxis.set_minor_locator(ticker.LogLocator(base=10.0, subs=np.arange(1, 10) * 0.1, numticks=100))
    axes[-1, col].grid(True, which='both', linestyle='--', linewidth=0.5)

plt.tight_layout()
plt.show()
