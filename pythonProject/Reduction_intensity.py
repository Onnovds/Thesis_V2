import numpy as np
import matplotlib.pyplot as plt
from scipy.optimize import curve_fit
from numpy.polynomial.polynomial import Polynomial

# Data protons
E_gen_protons = np.array([31.7, 32.5, 35, 40, 50, 100, 200])  # in MeV
Total_hits_detector = np.array([178, 75800, 99118, 100014, 100011, 100004, 100005])
Total_hits_generator = np.array([100000, 100000, 100000, 100000, 100000, 100000, 100000])
Total_secondaries_protons = np.array([0, 11, 113, 15, 11, 4, 5])
Hits_primaries = Total_hits_detector-Total_secondaries_protons
Transmission_factor = Total_hits_detector / Total_hits_generator
Reduction_factor = 1 - Transmission_factor
Secondary_factor_protons = Total_secondaries_protons / Total_hits_generator
print(1-Hits_primaries/Total_hits_generator)


# Data electrons
E_gen_e = np.array([1.0, 3.0, 5.0, 10.0, 20.0])  # in MeV
Total_hits_detector_e_all = np.array([1935, 29268, 116613, 142665, 152543])
Total_hits_generator_e_all = np.array([100000, 100000, 100000, 100000, 100000])
Total_Secondaries_e_all = np.array([1935, 16259, 32555, 43535, 52831])
Hits_primaries_e = Total_hits_detector_e_all-Total_Secondaries_e_all
Transmission_factor_e_all = Total_hits_detector_e_all / Total_hits_generator_e_all
Reduction_factor_e_all = 1 - Transmission_factor_e_all
Secondary_factor_e_all = Total_Secondaries_e_all / Total_hits_generator_e_all


# Data photons
E_gen_photons = np.array([0.01, 0.02, 0.05, 0.1, 0.5, 1.0])  # in MeV
Total_hits_detector_photons = np.array([0, 1581, 72988, 89857, 96484, 98188])
Total_hits_generator_photons = np.array([100000, 100000, 100000, 100000, 100000, 100000])
Total_Secondaries_photons = np.array([0, 376, 2784, 1509, 1026, 1147])
Hits_primaries_photons = Total_hits_detector_photons-Total_Secondaries_photons
Transmission_factor_photons = Total_hits_detector_photons / Total_hits_generator_photons
Reduction_factor_photons = 1 - Transmission_factor_photons
Secondary_factor_photons = Total_Secondaries_photons / Total_hits_generator_photons
print("primhits: ", Hits_primaries_photons)
print("prims: ", 1-Hits_primaries_photons/Total_hits_generator_photons)

# Inverse power law function 
def inverse_power_law(x, a, b, c):
    return a - (b / (x ** c))

# Saturation function for electron secondaries
def saturation_func(x, A, B):
    return A * (1 - np.exp(-B * x))

# Log-Normal function for Photon secondaries
def log_normal(x, A, mu, sigma):
    return A * np.exp(-((np.log(x) - mu) ** 2) / (2 * sigma ** 2))

initial_guess_e = [0.6, 0.1]
popt_e, pcov_e = curve_fit(saturation_func, E_gen_e, Secondary_factor_e_all, p0=initial_guess_e, maxfev=10000)

initial_guess_photons = [0.03, -2, 1]
popt_photons, pcov_photons = curve_fit(log_normal, E_gen_photons, Secondary_factor_photons, p0=initial_guess_photons, maxfev=10000)

# Fit cubic polynomial for intensity reduction
coeffs = np.polyfit(E_gen_protons, Reduction_factor, deg=4)
poly_func = np.poly1d(coeffs)

coeffs_e_all = np.polyfit(E_gen_e, Reduction_factor_e_all, deg=4)
poly_func_e_all = np.poly1d(coeffs_e_all)

coeffs_photons = np.polyfit(E_gen_photons, Reduction_factor_photons, deg=4)
poly_func_photons = np.poly1d(coeffs_photons)

# Print polynomial coefficients 
print(f"Cubic polynomial coefficients:\n{coeffs}")

# Plot lines
#protons intensity
x_fit_protons = np.linspace(min(E_gen_protons), max(E_gen_protons), 500)
y_fit_protons = poly_func(x_fit_protons)
#electrons intensity
x_fit_e = np.linspace(min(E_gen_e), max(E_gen_e), 500)
y_fit_e_all = poly_func(x_fit_e)
#photons intensity
x_fit_photons = np.linspace(min(E_gen_photons), max(E_gen_photons), 500)
y_fit_photons = poly_func(x_fit_photons)

# protons secondary
# Amount of secondary particles generated per primary particle so small that it's negligible --> with current physics_list

#electrons secondary
x_fit_secondary_e = np.linspace(min(E_gen_e), max(E_gen_e), 500)
y_fit_secondary_e = saturation_func(x_fit_secondary_e, *popt_e)

#photons secondary
x_fit_photons = np.linspace(min(E_gen_photons), max(E_gen_photons), 500)
y_fit_photons = log_normal(x_fit_photons, *popt_photons)

plt.figure(figsize=(10, 7))
plt.scatter(E_gen_e, Secondary_factor_e_all, color='green', label='Electron data')
plt.plot(x_fit_secondary_e, y_fit_secondary_e, label=f'Saturation Fit', color='black')
plt.xlabel('Electron Energy (MeV)')
plt.ylabel('Secondary Generation Factor')
plt.title('Saturation Fit of Electron Secondary Generation')
plt.legend()
plt.grid(True)

plt.figure(figsize=(10, 7))
plt.scatter(E_gen_photons, Secondary_factor_photons, color='orange', label='Photon data')
plt.plot(x_fit_photons, y_fit_photons, label=f'Log-normal Fit', color='black')
plt.xlabel('Photon Energy (MeV)')
plt.ylabel('Secondary Generation Factor')
plt.title('Log-Normal Fit of Photon Secondary Generation')
plt.legend()
plt.grid(True)



# Plot protons
plt.figure(figsize=(10, 7))
plt.scatter(E_gen_protons, Reduction_factor, color='red', label='Data points')
plt.plot(x_fit_protons, y_fit_protons, label=f'Fit: a={coeffs}')
plt.xlabel('Energy before shielding (MeV)')
plt.ylabel('Intensity Reduction Factor')
plt.title('Inverse Power Law Fit of Proton Intensity Reduction Factor due to 5mm Aluminium Shielding')
plt.legend()
plt.grid(True)

# Plot electrons
plt.figure(figsize=(10, 7))
plt.scatter(E_gen_e, Reduction_factor_e_all, color='blue', label='Electrons (all) data')
plt.plot(x_fit_e, y_fit_e_all, color='blue', linestyle='--', label=f'Electrons (all) fit: a={coeffs_e_all}')
plt.xlabel('Energy before shielding (MeV)')
plt.ylabel('Relative Intensity Reduction Factor')
plt.title('Inverse Power Law Fit of Electron Intensity Reduction Factor due to 5mm Aluminium Shielding')
plt.legend()
plt.grid(True)

# Plot photons
plt.figure(figsize=(10, 7))
plt.scatter(E_gen_photons, Reduction_factor_photons, color='orange', label='Data points')
plt.plot(x_fit_photons, y_fit_photons, label=f'Fit: a={coeffs_photons}')
plt.xlabel('Energy before shielding (MeV)')
plt.ylabel('Relative Intensity Reduction Factor')
plt.title('Inverse Power Law Fit of Photon Intensity Reduction Factor due to 5mm Aluminium Shielding')
plt.legend()
plt.grid(True)


plt.show()