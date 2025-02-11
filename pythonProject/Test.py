import pandas as pd
import matplotlib.pyplot as plt

# Load the data
data = pd.read_csv('filtered_output.txt', sep='\t')
# Verify the data types and check for any non-numeric entries
data['LET'] = pd.to_numeric(data['LET'], errors='coerce')
data['E'] = pd.to_numeric(data['E'], errors='coerce')
plt.figure(figsize=(10, 6))
plt.scatter(data['E']/1000, data['LET'], alpha=0.6, edgecolors='w', s=80)
plt.xlabel('E [MeV]')
plt.ylabel('LET')
plt.xscale('log')
plt.yscale('log')
plt.title('LET vs. E')
plt.grid(True)
plt.show()



