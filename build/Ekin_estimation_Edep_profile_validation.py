import uproot
import numpy as np
import matplotlib.pyplot as plt

# --- 1. Define Your Final Fit Model and Parameters ---
def exponential_decay_model(edep, a, b, c):
    # This function predicts Ekin from Edep
    return a * np.exp(-b * edep) + c

# !!! IMPORTANT: Replace these with the actual parameters from your fit !!!
fit_params = {
    'a': 172.95,   # Replace with your value
    'b': 0.0085,   # Replace with your value
    'c': 2.0      # Replace with your value
}

# --- 2. Load the 50 MeV Data ---
filename = "/home/onno/satellite_test/build/Protons/100kProton200MeV_Al_Slab_5mm_Vacuum_fStopandKill_Omnidirectional.root"
tree_name = "DetectorData"

try:
    with uproot.open(filename) as file:
        # Load both branches
        data = file[tree_name].arrays(['DepositedEnergy', 'KineticEnergy'], library="pd")
    print(f"Successfully loaded {len(data)} events.")
except Exception as e:
    print(f"Error loading ROOT file: {e}")
    exit()

# --- 3. Process Data ---
valid_data = data[(data['DepositedEnergy'] > 0) & (data['KineticEnergy'] > 0)].copy()
edep_data_kev = valid_data['DepositedEnergy'] * 1000
actual_ekin_data = valid_data['KineticEnergy']

# --- 4. Predict Ekin using your model ---
predicted_ekin_data = exponential_decay_model(edep_data_kev, **fit_params)

# --- 5. Create the Comparison Histogram Plot ---
plt.figure(figsize=(12, 8))

# Define common histogram settings for a fair comparison
bins = 100
plot_range = (0, 200) # Zoom in on the relevant peak region

# Plot the histogram of the ACTUAL kinetic energies (as a line)
plt.hist(actual_ekin_data, bins=bins, range=plot_range,
         histtype='step', linewidth=2, color='black',
         label='Actual Ekin Distribution (from Geant4)')

# Plot the histogram of the PREDICTED kinetic energies (as a filled area)
plt.hist(predicted_ekin_data, bins=bins, range=plot_range,
         color='red', alpha=0.7,
         label='Predicted Ekin Distribution (from Model)')

plt.title('Validation: Predicted vs. Actual Ekin Distribution for 200 MeV Run', fontsize=16)
plt.xlabel('Kinetic Energy (Ekin) [MeV]', fontsize=15)
plt.ylabel('Number of Events', fontsize=15)
plt.legend(fontsize=14)
plt.grid(True, axis='y', alpha=0.5)
plt.show()