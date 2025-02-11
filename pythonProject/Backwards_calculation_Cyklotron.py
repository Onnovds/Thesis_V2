import Bethe_bloch_helperfunctions as bb
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from SRIM_data import SRIM_data_Aluminium, SRIM_data_Silicon

# Load the data
data = pd.read_csv('Cyklotron_proton_20230324.txt', sep='\t')
data = data.apply(pd.to_numeric, errors='coerce')  # Convert all columns to numeric
LET_list_test = data['LET'].to_numpy()[:15]  # Convert to NumPy array

# Extract SRIM Data
SRIM_LET_data_Aluminium = np.array(SRIM_data_Aluminium)[78:144, 1]
SRIM_Ekin_data_Aluminium_MeV = np.array(SRIM_data_Aluminium)[78:144, 0] / 1e3

SRIM_LET_data_Silicon = np.array(SRIM_data_Silicon)[78:144, 1]
SRIM_Ekin_data_Silicon_MeV = np.array(SRIM_data_Silicon)[78:144, 0] / 1e3

# Compute beta and kinetic energy at detector (Vectorized)
beta_solution = bb.beta_from_LET_list(LET_list_test, "Silicon")  # Vectorized function
Ekin_list = bb.kinetic_energy(beta_solution)  # Vectorized function

# Preallocate memory for results
results_silicon = np.empty(len(LET_list_test), dtype=object)
results_air = np.empty(len(LET_list_test), dtype=object)

# Loop efficiently through LET values
for i, Ekin in enumerate(Ekin_list):
    # Compute values for Silicon (Avoid redundant calls)
    Ekin_values, LET_values, distance_values = bb.calculate_kinetic_energy_backward(Ekin, 'Silicon', 1, 1, 2e6+1)

    results_silicon[i] = {
        "KineticEnergies": Ekin_values,
        "LETValues": LET_values,
        "Distances": distance_values
    }

    # Retrieve last kinetic energy and distance
    last_Ekin_silicon = Ekin_values[-1]
    last_distance_silicon = distance_values[-1] * 1e4  # Convert to cm again

    # Compute values for Air (Avoid redundant calls)
    Ekin_values_air, LET_values_air, distance_values_air = bb.calculate_kinetic_energy_backward(
        last_Ekin_silicon, 'Air', 2e6, 10, last_distance_silicon
    )

    results_air[i] = {
        "KineticEnergies": Ekin_values_air,
        "LETValues": LET_values_air,
        "Distances": distance_values_air
    }

# Convert results to DataFrames (Faster access)
df_silicon = pd.DataFrame(results_silicon.tolist())
df_air = pd.DataFrame(results_air.tolist())


# Plot results
plt.figure(figsize=(10, 6))

# Plot all Silicon results efficiently
plt.xlabel("Distance from generator (cm)")
plt.ylabel("Kinetic Energy (keV)")

for _, row in df_silicon.head(100).iterrows():
    plt.plot(row['Distances'], row['KineticEnergies'], color="blue", alpha=0.5)

for i, row in df_air.head(100).iterrows():
    plt.plot(row['Distances'], row['KineticEnergies'], label=f"Result {i+1}", alpha=0.5)

plt.legend(loc="lower left", bbox_to_anchor=(0.01, 0.01))
plt.show()

Ekin_at_detector = df_silicon['KineticEnergies'].apply(lambda x: x[0])
Ekin_at_generator = df_air['KineticEnergies'].apply(lambda x: x[-1])

print(Ekin_at_generator)