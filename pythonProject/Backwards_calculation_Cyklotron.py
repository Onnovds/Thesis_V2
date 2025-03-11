import Bethe_bloch_helperfunctions as bb
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from SRIM_data import SRIM_data_Aluminium, SRIM_data_Silicon
import time

# Load the data
data = pd.read_csv('Cyklotron_proton_20230324.txt', sep='\t')
data = data.apply(pd.to_numeric, errors='coerce')  # Convert all columns to numeric
LET_list_test = data['LET'].to_numpy()[:]  # Convert to NumPy array

# Extract SRIM Data
SRIM_LET_data_Aluminium = np.array(SRIM_data_Aluminium)[78:144, 1]
SRIM_Ekin_data_Aluminium_MeV = np.array(SRIM_data_Aluminium)[78:144, 0] / 1e3

SRIM_LET_data_Silicon = np.array(SRIM_data_Silicon)[78:144, 1]
SRIM_Ekin_data_Silicon_MeV = np.array(SRIM_data_Silicon)[78:144, 0] / 1e3

start_time = time.time()

#------------------------------------------ Quick test of old code vs vectorised code ------------------------------------------
# Compute beta and kinetic energy at detector (non-vectorised)
#beta_solution = bb.beta_from_LET_list(LET_list_test, "Silicon")  # Non-vectorised function
#Ekin_list = bb.kinetic_energy(beta_solution)  


# Compute beta and kinetic energy at detector (Vectorised)
Correction_factor_Cyklotron = 1.0
# Set values < 0.3 to 0    --- when looking in data, many values <0.1 but a few around 1.5 so chose 0.3 to be safe
LET_list_test[LET_list_test < 0.3] = 0
LET_list_test = LET_list_test * Correction_factor_Cyklotron  # Apply correction factor

beta_solution_vectorised = bb.beta_from_LET_list_Silicon_vectorised(LET_list_test)  # Vectorised function
Ekin_list_vectorised = bb.kinetic_energy(beta_solution_vectorised)  # Vectorised function

# Preallocate memory for results
results_silicon_vectorised = np.empty(len(LET_list_test), dtype=object)
results_air_vectorised = np.empty(len(LET_list_test), dtype=object)

Ekin_values0 = bb.calculate_kinetic_energy_backward_optimised2(Ekin_list_vectorised, 'Silicon', 2, 1, 2e6+1)
print("test", Ekin_values0)

batch_size = 1000  # Process 500 particles at a time
Ekin_final_results = []

for start in range(0, len(Ekin_values0), batch_size):
    batch = Ekin_values0[start : start + batch_size]
    batch_results = bb.calculate_kinetic_energy_backward_optimised2(batch, "Air", 2e6, 10, 2e6)
    Ekin_final_results.append(batch_results)

Ekin_final_results = np.concatenate(Ekin_final_results)  # Combine all batches

end_time = time.time()
print(f"Execution time: {end_time - start_time:.4f} seconds")

#print(Ekin_final_results)

import pandas as pd

df = pd.DataFrame({"Ekin_final (keV)": Ekin_final_results})
df.to_csv("Ekin_calculated_Cyklotron_Cfactor1.0_.csv", index=False)





'''

# Loop efficiently through LET values
for i, Ekin in enumerate(Ekin_list_vectorised):
    # Compute values for Silicon (Avoid redundant calls)
    Ekin_values, LET_values, distance_values = bb.calculate_kinetic_energy_backward_vectorised(Ekin, 'Silicon', 10, 1, 2e6+1)

    results_silicon_vectorised[i] = {
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

    results_air_vectorised[i] = {
        "KineticEnergies": Ekin_values_air,
        "LETValues": LET_values_air,
        "Distances": distance_values_air
    }

# Convert results to DataFrames (Faster access)
df_silicon_vectorised = pd.DataFrame(results_silicon_vectorised.tolist())
df_air_vectorised = pd.DataFrame(results_air_vectorised.tolist())

Ekin_at_detector_vectorised = df_silicon_vectorised['KineticEnergies'].apply(lambda x: x[0])
Ekin_at_generator_vectorised = df_air_vectorised['KineticEnergies'].apply(lambda x: x[-1])

print(Ekin_at_generator_vectorised)

#-------------------------------------------------------------------------------------------------------------------------------
'''

'''

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
'''