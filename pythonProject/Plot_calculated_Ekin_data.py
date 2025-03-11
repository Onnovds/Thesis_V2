import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

# Load the data
data = pd.read_csv('Cyklotron_proton_20230324.txt', sep='\t')
data = data.apply(pd.to_numeric, errors='coerce')  # Convert all columns to numeric
LET_list_data = data['LET'].to_numpy()[:]  # Convert to NumPy array

print(max(LET_list_data))

# Load CSV while keeping missing values as NaN
df = pd.read_csv("Ekin_calculated_Cyklotron_Cfactor1.55_.csv")

# Convert column to numeric, keeping NaN values
df["Ekin_final (keV)"] = pd.to_numeric(df["Ekin_final (keV)"], errors='coerce')

# Convert to NumPy array (preserves NaN values)
Ekin_final = df["Ekin_final (keV)"].to_numpy()

# Define variable bin edges
bins = np.concatenate([
    np.arange(20000, 50000, 500),  # Small bins in main range (250 keV width)
    np.arange(50000, 100000, 2000),  # Medium bins (5000 keV width)
    np.arange(100000, 300000, 199999)  # Large bins for outliers (20,000 keV width)
])

# Plot histogram
plt.figure(0)
plt.hist(Ekin_final[~np.isnan(Ekin_final)], bins=bins)
plt.xlim(0, 300000)
plt.title("Calculated kinetic energy at generator for all data points")
plt.xlabel("Ekin at generator [keV]")
plt.ylabel("Frequency [-]")

plt.figure(1)
plt.plot(LET_list_data, Ekin_final,  'o')
plt.ylim(0, 1000000)
plt.title("Calculated kinetic energy at generator vs LET at detector")
plt.ylabel("Ekin at generator [keV]")
plt.xlabel("LET at detector [keV/um]")

plt.show()

