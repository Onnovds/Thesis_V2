import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

# --- 1. Load and Bin Your Data ---
try:
    filename = 'profile_data_all.csv'
    data = pd.read_csv(filename)
    ekin_raw = data['Ekin_MeV'].values
    edep_raw = data['Avg_Edep_keV'].values
    print(f"Successfully loaded {len(data)} raw data points.")
except FileNotFoundError:
    print(f"Error: {filename} not found.")
    exit()

df = pd.DataFrame({'Ekin': ekin_raw, 'Edep': edep_raw})
num_bins = 200 # Using a high number of bins for good resolution
bins = np.linspace(df['Ekin'].min(), df['Ekin'].max(), num_bins)
df['binned_ekin'] = pd.cut(df['Ekin'], bins=bins, labels=False, include_lowest=True)

# Use .agg() to take the mean of Ekin but the MAX of Edep for each bin
binned_data = df.groupby('binned_ekin').agg(
    Ekin=('Ekin', 'mean'),
    Edep=('Edep', 'max')
).dropna()

ekin_binned = binned_data['Ekin'].values
edep_binned = binned_data['Edep'].values
print(f"Data smoothed into {len(binned_data)} points. This is our final model.")

# --- 2. Build the Look-Up Tool Directly from Binned Data ---
# Find the Bragg Peak in the binned data
peak_idx = np.argmax(edep_binned)

# Split the binned data into the two regions for interpolation
ekin_stopping = ekin_binned[:peak_idx+1]
edep_stopping = edep_binned[:peak_idx+1]

ekin_penetrating = ekin_binned[peak_idx:]
edep_penetrating = edep_binned[peak_idx:]

def find_ekin_solutions(edep_measured):
    """
    Finds the possible Ekin solutions for a given Edep by interpolating
    on the binned simulation data.
    """
    sol1, sol2 = None, None
    # Solution for the stopping region (low Ekin)
    if edep_measured <= max(edep_stopping):
        sol1 = np.interp(edep_measured, edep_stopping, ekin_stopping)
    # Solution for the penetrating region (high Ekin)
    if edep_measured <= max(edep_penetrating):
        # np.interp needs increasing x-values, so we flip the penetrating arrays
        sol2 = np.interp(edep_measured, edep_penetrating[::-1], ekin_penetrating[::-1])
    return sol1, sol2

# --- 3. Example Usage ---
# This is how you would process your full satellite dataset
# For example:
# satellite_edep_data = np.array([1500, 2500, 1850, 2100, ...]) 
# final_ekin_estimates = []
# for edep_event in satellite_edep_data:
#     stopping_sol, _ = find_ekin_solutions(edep_event)
#     final_ekin_estimates.append(stopping_sol)

# Let's test with a single value
satellite_edep = 2500
ekin_sol1, ekin_sol2 = find_ekin_solutions(satellite_edep)

print("\n--- Look-up Tool Results ---")
print(f"For a measured Edep of {satellite_edep} keV, the possible Ekin solutions are:")
if ekin_sol1: print(f"  1. Stopping Particle Solution: {ekin_sol1:.2f} MeV")
else: print(f"  1. No solution in stopping region (Edep is likely too high).")
if ekin_sol2: print(f"  2. Penetrating Particle Solution: {ekin_sol2:.2f} MeV")
else: print(f"  2. No solution in penetrating region.")

# --- 4. Visualise the Model ---
plt.figure(figsize=(12, 8))
plt.scatter(ekin_raw, edep_raw, label='Raw Data Points', alpha=0.2, color='gray')
plt.plot(ekin_binned, edep_binned, '-o', label='Binned Data (The Lookup Model)', color='blue')
plt.title('Model for Look-up and Interpolation')
plt.xlabel('Kinetic Energy (Ekin) [MeV]')
plt.ylabel('Deposited Energy (Edep) [keV]')
plt.legend()
plt.grid(True)




# --- 5. NEW PLOT 1: Raw Data Scatter Plot ---
plt.figure(figsize=(12, 8))
plt.scatter(ekin_raw, edep_raw, alpha=0.8, color='gray')
plt.title('Raw Simulation Data: Edep vs Ekin')
plt.xlabel('Kinetic Energy (Ekin) [MeV]')
plt.ylabel('Deposited Energy (Edep) [keV]')
plt.grid(True)

# --- 6. NEW PLOT 2: Raw Data with Exponential Fit ---

# Define the model function you provided
def exponential_decay_model(edep, a, b, c):
    # This function predicts Ekin from Edep
    return a * np.exp(-b * edep) + c

# Your fitted parameters
fit_params = {
    'a': 220,
    'b': 0.011,
    'c': 2.0
}

# Create a new figure
plt.figure(figsize=(12, 8))

# Plot the raw data again
plt.scatter(edep_raw, ekin_raw, alpha=0.2, color='gray', label='Raw Data Points')

# To plot your function, we generate a smooth range of Edep values
edep_for_plot = np.linspace(min(edep_raw), max(edep_raw), 500)

# Calculate the predicted Ekin for each of these Edep values
ekin_predicted = exponential_decay_model(edep_for_plot, **fit_params)

# Plot the model's prediction (Ekin on x-axis, Edep on y-axis)
plt.plot(edep_for_plot, ekin_predicted, color='red', linewidth=3, label='Exponential Fit Model')

# Add titles and labels
plt.title('Model: Exponential Decay Fit vs Raw Data')
plt.xlabel('Deposited Energy (Edep) [keV]')
plt.ylabel('Kinetic Energy (Ekin) [MeV]')
plt.legend()
plt.grid(True)

plt.show()