import numpy as np
import matplotlib.pyplot as plt
from scipy.optimize import curve_fit
from numpy.polynomial.polynomial import Polynomial
from JoeySat_GetData import arrays_23, arrays_24


#Data JoeySat
Edep_23_protons = arrays_23["class1"][:,5]              # Energy deposition
Edep_24_protons = arrays_24["class1"][:,5]
Edep_23_electrons = arrays_23["class2"][:,5]
Edep_24_electrons = arrays_24["class2"][:,5]

# Data protons
E_gen = np.array([31.7, 32.5, 35, 40, 50, 100, 200])  # in MeV
E_detector = np.array([1.08, 2.57, 11.09, 20.88, 35.34, 92.11, 195.20])
Ekin_modal_detector = np.array([0.43, 2.0, 9.97, 20.26, 34.13, 91.42, 194.39])
E_dep_mean = np.array([1025.08, 2020.78, 1841.57, 1603.95, 1166.85, 638.81, 402.82])  # in keV
E_dep_peak = np.array([920.07, 3074.38, 2664.02, 2273.91, 1559.71, 690.51, 372.28]) #in keV
E_dep_random = Edep_23_protons
E_reduction = E_gen - Ekin_modal_detector  # Energy reduction due to shielding
print(E_reduction)
Transmission_factor = np.array([0.0340694, 0.07907692, 0.31685714, 0.522, 0.7068, 0.9211, 0.976])  #from table in overleaf --> mean Ekin in detector/mean Ekin at generator
Reduction_factor = 1-Transmission_factor

# Data electrons
E_gen_e = np.array([1.0, 3.0, 5.0, 10.0, 20.0])  # in MeV
E_detector_e_all = np.array([0.15, 0.45, 1.94, 5.43, 11.69]) # in MeV
E_detector_e_primaries = np.array([0.0, 0.67, 2.38, 7.40, 16.92])
Reduction_factor_e_all = 1 - E_detector_e_all / E_gen_e
Reduction_factor_e_primaries = 1 - E_detector_e_primaries / E_gen_e 
print(Reduction_factor_e_all)
print(Reduction_factor_e_primaries)

# Data photons
E_gen_photons = np.array([0.01, 0.02, 0.05, 0.1, 0.5, 1.0])  # in MeV
E_detector_photons = np.array([0.0, 0.019, 0.049, 0.098, 0.49, 0.98]) # in MeV
Reduction_factor_photons = 1 - E_detector_photons / E_gen_photons


# Inverse power law function
def inverse_power_law(x, a, b, c):
    return a - (b / (x ** c))

def power_law_with_offset(x, a, b, c):
    return a * (x ** b) + c

# Initial guess
initial_guess_protons = [-1, 1, 35]
initial_guess_e_all = [0.1, 1, 1.5]
initial_guess_e_primaries = [0.1,1, 1.5]
initial_guess_photons = [1, 1e4, 2]

# Fit
popt_protons, pcov_protons = curve_fit(power_law_with_offset, Ekin_modal_detector, E_gen, p0=initial_guess_protons)
popt_e_all, pcov_e_all = curve_fit(inverse_power_law, E_gen_e, Reduction_factor_e_all, p0=initial_guess_e_all, maxfev=10000) #upped the maxfev to give it more chance to converge
popt_e_primaries, pcov_e_primaries = curve_fit(inverse_power_law, E_gen_e, Reduction_factor_e_primaries, p0=initial_guess_e_primaries, maxfev=10000)
popt_photons, pcov_photons = curve_fit(inverse_power_law, E_gen_photons, Reduction_factor_photons, p0=initial_guess_photons)

# Print parameters
print(f"Fit parameters:\na = {popt_protons[0]:.3f}, b = {popt_protons[1]:.3f}, c = {popt_protons[2]:.3f}")

# Plot
#protons
x_fit_protons = np.linspace(min(Ekin_modal_detector), max(Ekin_modal_detector), 500)
y_fit_protons = power_law_with_offset(x_fit_protons, *popt_protons)
#electrons
x_fit_e = np.linspace(min(E_gen_e), max(E_gen_e), 500)
y_fit_e_all = inverse_power_law(x_fit_e, *popt_e_all)
y_fit_e_prim = inverse_power_law(x_fit_e, *popt_e_primaries)
#photons
x_fit_photons = np.linspace(min(E_gen_photons), max(E_gen_photons), 500)
y_fit_photons = inverse_power_law(x_fit_photons, *popt_photons)

Predicted_Ekin = power_law_with_offset(Ekin_modal_detector, *popt_protons)
# Print predicted Ekin values
print("Predicted Ekin values after shielding:")
for Ekin in Predicted_Ekin:
    print(f"{Ekin:.2f} MeV")
    
percentage_errors = ((Predicted_Ekin - E_gen) / E_gen) * 100
mean_abs_percentage_error = np.mean(np.abs(percentage_errors))
mean_abs_error = np.mean(np.abs(Predicted_Ekin - E_gen))
print(f"Mean Absolute Error: {mean_abs_error:.2f}%")

print(percentage_errors)
print(f"Mean Absolute Percentage Error: {mean_abs_percentage_error:.2f}%")




# Plot protons
plt.figure(figsize=(10, 7))
plt.scatter(Ekin_modal_detector, E_gen, color='red', label='Data points')
plt.plot(x_fit_protons, y_fit_protons, label=f'Fit: a={popt_protons[0]:.2f}, b={popt_protons[1]:.1f}, c={popt_protons[2]:.2f}')
plt.xlabel('Modal Kinetic Energy after shielding (MeV)', fontsize=14)
plt.ylabel('Energy before shielding (MeV)', fontsize=14)
plt.title('Power Law Fit of Proton Energy Reduction due to 5mm Aluminium Shielding', fontsize=16)
plt.legend()
plt.grid(True)

# Plot electrons
plt.figure(figsize=(10, 7))
plt.scatter(E_gen_e, Reduction_factor_e_all, color='blue', label='Electrons (all) data')
plt.plot(x_fit_e, y_fit_e_all, color='blue', linestyle='--', label=f'Electrons (all) fit: a={popt_e_all[0]:.2f}, b={popt_e_all[1]:.1f}, c={popt_e_all[2]:.2f}')

plt.scatter(E_gen_e, Reduction_factor_e_primaries, color='green', label='Electrons (primaries) data')
plt.plot(x_fit_e, y_fit_e_prim, color='green', linestyle='--', label=f'Electrons (primaries) fit: a={popt_e_primaries[0]:.2f}, b={popt_e_primaries[1]:.1f}, c={popt_e_primaries[2]:.2f}')

plt.xlabel('Energy before shielding (MeV)')
plt.ylabel('Relative Energy Reduction Factor')
plt.title('Inverse Power Law Fit of Electron Energy Reduction Factor due to 5mm Aluminium Shielding')
plt.legend()
plt.grid(True)

# Plot photons
plt.figure(figsize=(10, 7))
plt.scatter(E_gen_photons, Reduction_factor_photons, color='orange', label='Data points')
plt.plot(x_fit_photons, y_fit_photons, label=f'Fit: a={popt_photons[0]:.2f}, b={popt_photons[1]:.1f}, c={popt_photons[2]:.2f}')
plt.xlabel('Energy before shielding (MeV)')
plt.ylabel('Relative Energy Reduction Factor')
plt.title('Inverse Power Law Fit of Photon Energy Reduction Factor due to 5mm Aluminium Shielding')
plt.legend()
plt.grid(True)


plt.show()



# ===== Detector Energy → Initial Energy Fit =====

# ===== Polynomial Fit =====
# Fit 2nd degree polynomial (you can increase to 3 if needed)
coeffs = np.polyfit(E_detector, E_gen, deg=2)
poly_func = np.poly1d(coeffs)

# Print polynomial
print(f"Polynomial coefficients:\n{coeffs}")

# Plot
x_fit_protons = np.linspace(min(E_detector), max(E_detector), 500)
y_fit_protons = poly_func(x_fit_protons)

plt.figure(figsize=(8, 6))
plt.scatter(E_detector, E_gen, color='blue', label='Data points')
plt.plot(x_fit_protons, y_fit_protons, label=f'2nd Degree Polynomial Fit')
plt.xlabel('Detected Energy after shielding (MeV)')
plt.ylabel('Initial Kinetic Energy (MeV)')
plt.title('Polynomial Fit: Detected Energy → Initial Kinetic Energy')
plt.legend()
plt.grid(True)
plt.show()


