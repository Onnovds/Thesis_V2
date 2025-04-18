from JoeySat_GetData import arrays_23, arrays_24
import Bethe_bloch_helperfunctions as bb
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from scipy.interpolate import interp1d

#Create interpolation for electrons LET to Ekin
Electron_in_Silicon = np.array(pd.read_csv("/home/onno/satellite_test/pythonProject/Electrons_in_Silicon_10keV_1GeV_ESTAR.txt", sep='\s+', header=2))
Electron_in_Aluminium = np.array(pd.read_csv("/home/onno/satellite_test/pythonProject/Electrons_in_Aluminium_10keV_1GeV_ESTAR.txt", sep='\s+', header=2))
LET_to_Ekin_electrons_aluminium = bb.LET_to_Ekin_function(Electron_in_Aluminium[:,3]*2.7/10, Electron_in_Aluminium[:,0]) #*2.7/10 to convert from MeVcm2/g to keV/um
LET_to_Ekin_electrons_silicon = bb.LET_to_Ekin_function(Electron_in_Silicon[:,3]*2.33/10, Electron_in_Silicon[:,0]) #*2.33/10 to convert from MeVcm2/g to keV/um

# Generate a preset list of kinetic energies for a range of stopping power values
#min_LET = 
#LET_range = np.linspace(min(LET), max(LET) + 5, 500)  # Generate 100 values including extrapolation

LET_23_protons = arrays_23["class1"][:,5]/500           # Convert from MeVcm2/g to keV/um
LET_24_protons = arrays_24["class1"][:,5]/500
LET_23_electrons = arrays_23["class2"][:,5]/500
LET_24_electrons = arrays_24["class2"][:,5]/500

Edep_23_protons = arrays_23["class1"][:,5]              # Energy deposition
Edep_24_protons = arrays_24["class1"][:,5]
Edep_23_electrons = arrays_23["class2"][:,5]
Edep_24_electrons = arrays_24["class2"][:,5]


Flux_23_protons = arrays_23["class1"][:,3]              # Protons + Ions  fluxes
Flux_24_protons = arrays_24["class1"][:,3]


beta_solution_vectorised_23 = bb.beta_from_LET_list_Silicon_vectorised(LET_23_protons)  # Vectorised function
beta_solution_vectorised_24 = bb.beta_from_LET_list_Silicon_vectorised(LET_24_protons)  # Vectorised function

#print("beta_solution_vectorised_23", beta_solution_vectorised_23)
Ekin_list_vectorised_23 = bb.kinetic_energy(beta_solution_vectorised_23)  # Vectorised function
Ekin_list_vectorised_24 = bb.kinetic_energy(beta_solution_vectorised_24)  # Vectorised function
#Ekin_list_electrons_23 = bb.LET_to_Ekin_function(LET_23_electrons, "Silicon")

print("length = ", len(Ekin_list_vectorised_23))

# Preallocate memory for results 
results_silicon_vectorised_23 = np.empty(len(LET_23_protons), dtype=object)
results_aluminium_vectorised_23 = np.empty(len(LET_23_protons), dtype=object)
results_silicon_vectorised_24 = np.empty(len(LET_24_protons), dtype=object)
results_aluminium_vectorised_24 = np.empty(len(LET_24_protons), dtype=object)

#Find kinetic energy at detector
Ekin_values0_23 = bb.calculate_kinetic_energy_backward_optimised2(Ekin_list_vectorised_23, 'Silicon', 2, 1, 5e3+1)
Ekin_values0_24 = bb.calculate_kinetic_energy_backward_optimised2(Ekin_list_vectorised_24, 'Silicon', 2, 1, 5e3+1)

batch_size = 500  # Process 500 particles at a time --> not important for this case as only 204 datapoints
Ekin_final_results_23 = []
Ekin_final_results_24 = []

for start in range(0, len(Ekin_values0_23), batch_size):
    batch = Ekin_values0_23[start : start + batch_size]
    batch_results = bb.calculate_kinetic_energy_backward_optimised2(batch, "Aluminium", 5e3, 1, 5e3)
    Ekin_final_results_23.append(batch_results)

for start in range(0, len(Ekin_values0_24), batch_size):
    batch = Ekin_values0_24[start : start + batch_size]
    batch_results = bb.calculate_kinetic_energy_backward_optimised2(batch, "Aluminium", 5e3, 1, 5e3)
    Ekin_final_results_24.append(batch_results)

Ekin_final_results_23 = np.concatenate(Ekin_final_results_23)  # Combine all batches
Ekin_final_results_24 = np.concatenate(Ekin_final_results_24)  # Combine all batches

print("test", Ekin_final_results_24)

min_detector = min(Ekin_values0_23)
max_detector = max(Ekin_values0_23)
print("min", min_detector)
print("max", max_detector)
num_bins = 30  # Adjust for smoothness
Median_ekin_detector = np.median(Ekin_values0_23)

bins_detector= np.logspace(np.log10(min_detector), np.log10(max_detector), num_bins)
hist, bins = np.histogram(Ekin_values0_23, bins=bins_detector)

min_before_shielding = min(Ekin_final_results_23)
max_before_shielding = max(Ekin_final_results_23)
num_bins = 30  # Adjust for smoothness
Median_ekin_before_shielding = np.median(Ekin_final_results_23)

bins_before_shielding = np.logspace(np.log10(min_before_shielding), np.log10(max_before_shielding), num_bins)

print("min", min_before_shielding)
print("max", max_before_shielding)

Ekin_values_after_shielding = np.concatenate((Ekin_values0_23, Ekin_values0_24))
Ekin_values_before_shielding = np.concatenate((Ekin_final_results_23, Ekin_final_results_24))
min_Ekinvalue_23_24march = min(Ekin_values_after_shielding)
max_Ekinvalue_23_24march = max(Ekin_values_before_shielding)
bins_for_plotting =  np.logspace(np.log10(min_Ekinvalue_23_24march), np.log10(max_Ekinvalue_23_24march), num=60)
print("length", len(Ekin_values_after_shielding))

plt.figure(1)
plt.hist(Ekin_values0_23, bins=bins_detector, histtype='step', color='blue', label="At detector after shielding")
plt.hist(Ekin_final_results_23, bins=bins_before_shielding, histtype='step', color='red', label="Before shielding")
plt.xscale('log')
plt.xlabel('Kinetic energy [keV]')
plt.ylabel('Counts [#]')
plt.title('Kinetic Energy Spectrum of class 1 (Protons + Ions) - 23 March 2024')
plt.legend()
plt.grid(True)
plt.xlim(9e2, 6e5)

print("max 24 after", max(Ekin_values0_24))
print("max 24 before", max(Ekin_final_results_24))

plt.figure(2)
plt.hist(Ekin_values0_24, bins=bins_for_plotting, histtype='step', color='blue', label="At detector after shielding")
plt.hist(Ekin_final_results_24, bins=bins_for_plotting, histtype='step', color='red', label="Before shielding")
plt.xscale('log')
plt.xlabel('Kinetic energy [keV]')
plt.ylabel('Counts')
plt.title('Kinetic Energy Spectrum of class 1 (Protons + Ions) - 24 March 2024')
plt.legend()
plt.grid(True)
plt.xlim(9e2, 6e6)

plt.figure(3)
plt.hist(Ekin_values_after_shielding, bins=bins_for_plotting, histtype='step', color='blue', label="At detector after shielding")
plt.hist(Ekin_values_before_shielding, bins=bins_for_plotting, histtype='step', color='red', label="Before shielding")
plt.xscale('log')
plt.xlabel('Kinetic energy [keV]', fontsize=14)
plt.ylabel('Counts [#]', fontsize=14)
plt.title('Kinetic Energy Spectrum of class 1 (Protons + Ions) - 23+24 March 2024', fontsize=16)
plt.legend()
plt.xticks(fontsize=13)
plt.yticks(fontsize=13)
plt.grid(True)
plt.xlim(9e2, 6e5)


plt.figure(4)
plt.scatter(Ekin_final_results_23/1e3, Flux_23_protons, label="23 march")
plt.scatter(Ekin_final_results_24/1e3, Flux_24_protons, label="24 march")
plt.yscale('log')
plt.xscale('log')
plt.xlabel('Kinetic energy [keV]')
plt.ylabel('Flux [cm^-2 s^-1]')
plt.legend()
#plt.show()


'''

Edep_after_shielding_protons = np.concatenate((Edep_23_protons, Edep_24_protons))
Edep_to_Ekin_before_shielding = interp1d(Edep_after_shielding_protons, Ekin_values_before_shielding/1e3, fill_value='extrapolate')
deg = len(Edep_after_shielding_protons) - 1  
coeffs = np.polyfit(Edep_after_shielding_protons, Ekin_values_before_shielding/1e3, deg=deg)  # Linear fit
poly = np.poly1d(coeffs)
print(poly)

Edep_random_value_range = np.linspace(170, 2e4, 1500)
Ekin_values_before_shielding_random = Edep_to_Ekin_before_shielding(Edep_random_value_range)
Ekin_values_before_shielding_poly = poly(Edep_random_value_range)
print(Ekin_values_before_shielding_poly[-1])

plt.figure(5)
plt.scatter(Edep_23_protons, Ekin_final_results_23/1e3, label="23 march")
plt.scatter(Edep_24_protons, Ekin_final_results_24/1e3, label="24 march")
plt.plot(Edep_random_value_range, Ekin_values_before_shielding_random, label="Random values")
plt.plot(Edep_random_value_range, Ekin_values_before_shielding_poly, label="Linear fit")
plt.legend()
plt.yscale('log')
plt.xscale('log')
plt.xlabel('Energy deposition [MeV]')
plt.ylabel('Calculated kinetic energy [MeV]')
plt.show()
'''

Geant4_generator_energy = np.array([31.7, 32.5, 35, 40, 50, 100, 200])
Geant4_detector_energy = np.array([1.08, 2.57, 11.09, 20.88, 35.34, 92.11, 195.2])
Geant4_coefficients = Geant4_detector_energy/Geant4_generator_energy
print(Geant4_coefficients)