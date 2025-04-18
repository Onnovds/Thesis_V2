import numpy as np
import matplotlib.pyplot as plt
import pandas as pd

# Define the filename
filename_CRRESPRO_active = "/home/onno/satellite_test/pythonProject/SPENVIS data/CRRESPRO_active_608km_circular_polar.txt"
filename_AP8_max = "/home/onno/satellite_test/pythonProject/SPENVIS data/AP-8_solarmaximum_608km_circular_polar.txt"

# Read the data correctly
data_CRRESPRO_active = pd.read_csv(filename_CRRESPRO_active, sep='\s+', skiprows=1, header=None)
data_AP8_max = pd.read_csv(filename_AP8_max, sep='\s+', skiprows=1, header=None)

# Convert to NumPy array
data_array_CRRESPRO_active = data_CRRESPRO_active.to_numpy()
data_array_AP8_max = data_AP8_max.to_numpy()

plt.plot(data_array_CRRESPRO_active[:, 0], data_array_CRRESPRO_active[:, 1], label="CRRESPRO active")
plt.plot(data_array_AP8_max[:, 0], data_array_AP8_max[:, 1], label="AP-8 solarmaximum")
plt.xlabel("Energy [MeV]")
plt.ylabel("Flux [cm^-2 s^-1]")
plt.yscale('log')
plt.xscale('log')
plt.title("Integral flux in 608km circular polar orbit for SPENVIS models")
plt.legend()

# energies and fluxes from CRRESPRO active model (608km circular polar)
energies = np.array([1.500000e+0, 2.000000e+0, 3.000000e+0, 4.000000e+0, 5.000000e+0, 6.000000e+0, 7.000000e+0, 1.000000e+1, 1.500000e+1, 2.000000e+1, 3.000000e+1, 4.000000e+1, 5.000000e+1, 6.000000e+1, 7.000000e+1])
avg_fluxes = np.array([5.226100e+2,	4.397100e+2,	3.261500e+2,	2.847000e+2,	2.631800e+2,	2.459000e+2,	2.299900e+2,	1.951600e+2,	1.604500e+2,	1.438600e+2,	1.177200e+2,	9.106200e+1,	7.561500e+1,	5.589500e+1,	3.382800e+1])

#print(np.mean(avg_fluxes))
	
#plt.figure()
#plt.plot(energies, avg_fluxes, marker='o', linestyle='-', color='b', label="Average Flux")
plt.show()