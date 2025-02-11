import pandas as pd
import matplotlib.pyplot as plt
import numpy as np


def read_data(file_path, columns_to_keep, new_column_names):
    # Read the file
    data = pd.read_csv(file_path, sep='\s+', header=0, names=new_column_names)

    # Keep only the specified columns
    data = data.iloc[:, columns_to_keep]

    return data


def plot_hits_2d(data, x_column, y_column, x_bins, y_bins):
    x = data[x_column]
    y = data[y_column]
    plt.figure(figsize=(10, 10))
    # Create a 2D histogram
    plt.hist2d(x, y, bins=[x_bins, y_bins], cmap='viridis', cmin=1)
    plt.colorbar(label='Number of Hits')
    plt.title("Hits on Detector Coordinates")
    plt.xlabel(x_column)
    plt.ylabel(y_column)
    plt.grid(False)
    plt.show()


def plot_LET(data, x_column, y_column):
    x = data[x_column]
    y = data[y_column]
    plt.figure(figsize=(10, 6))
    plt.scatter(x, y, s=1, alpha=0.5)
    plt.title("LET in Si 500um")
    plt.xlabel("LET [keV/um]")
    plt.ylabel(y_column)
    plt.xscale('log')
    plt.show()

def plot_E(data, x_column, y_column):
    x = data[x_column]
    y = data[y_column]
    plt.figure(figsize=(10, 6))
    plt.scatter(x, y, s=1, alpha=0.5)
    plt.title("Energy")
    plt.xlabel("Energy [keV]")
    plt.ylabel(y_column)
    plt.xscale('log')
    plt.show()

def plot_histogram(data, column_name, bin_edges, title=None, xlabel=None, ylabel='N[-]'):
    fig, ax1 = plt.subplots(figsize=(10, 12), nrows=2, sharex=True)

    plt.xscale('log')
    plt.xlim([1e-1, 2e2])

    # Top plot with linear y-axis
    ax1[0].hist(data[column_name], bins=bin_edges, edgecolor='black', alpha=0.7)
    ax1[0].set_title(title or f'Histogram of {column_name} (Linear Scale)')
    ax1[0].set_ylabel(ylabel)
    ax1[0].grid(True)

    # Bottom plot with logarithmic y-axis
    ax1[1].hist(data[column_name], bins=bin_edges, edgecolor='black', alpha=0.7)
    ax1[1].set_title(f'Histogram of {column_name} (Log Scale)')
    ax1[1].set_xlabel(xlabel or column_name)
    ax1[1].set_ylabel(ylabel)
    ax1[1].set_yscale('log')
    ax1[1].grid(True)

    plt.tight_layout()
    plt.show()

x = 0.001
nBins = 280
binEdges = np.zeros(nBins)
for i in range(nBins):
    binEdges[i] = x
    x1 = x + x*0.05
    x = x1

if __name__ == "__main__":
    file_path = "/home/onno/project_test/pythonProject/filtered_output.txt"  # Replace with your file path
    columns_to_keep = [0, 1, 2, 3, 4, 5, 6]  # Adjust based on your file's structure (0-based index)
    new_column_names = ["DetectorID", "EventID", "X", "Y", "E", "EpixMean", "LET"]  # Adjust based on your file's structure

    # Read the data
    data = read_data(file_path, columns_to_keep, new_column_names)

    #print("Columns in the data:", data.columns.tolist())
    data_array = np.array(data)

    # Plot
    #plot_hits_2d(data, "X", "Y", 256, 256)
    #plot_histogram(data, "LET", binEdges, title="Histogram of LET", xlabel='LET [keV/um]')
    #plot_E(data, "E", "EventID")
    # Plot LET over E

