import numpy as np
import matplotlib.pyplot as plt
from scipy.optimize import curve_fit
from numpy.polynomial.polynomial import Polynomial

# Data protons Omnidirectional source
E_gen_protons = np.array([31.7, 32.5, 35, 40, 50, 100, 200])  # Generator energy of protons in MeV
Total_protons = np.array([1, 1047, 12975, 30902, 45119, 45875, 46158]) #Total protons (both primaries and secondaries) in detector after shielding
Total_hits_detector = np.array([1565, 2680, 14937, 33373, 47993, 48412, 48664]) #Total hits in detector after shielding
Total_hits_noshield = np.array([46258, 46271, 46267, 46247, 46233, 46276, 46227])  #Total hits in detector with no shielding (100k generated)
Total_secondaries_noshield = np.array([19, 19, 19, 13, 13, 21, 19]) #Total secondaries in detector with no shielding
Total_secondaries_protons = np.array([1564, 1633, 1967, 2497, 2963, 2920, 3165]) #Total secondaries in detector with shielding
Total_hits_detector_V3 = np.array([178, 75800, 99118, 100014, 100011, 100004, 100005])
Total_hits_generator_V3 = np.array([100000, 100000, 100000, 100000, 100000, 100000, 100000])
Total_secondaries_protons_V3 = np.array([0, 11, 113, 15, 11, 4, 5])
Hits_primaries = Total_hits_detector-Total_secondaries_protons
Hits_primaries_noshield = Total_hits_noshield - Total_secondaries_noshield
Transmission_factor_total = Total_hits_detector / Total_hits_noshield       # Total hits in detector with shielding / Total hits in detector with no shielding
Transmission_factor_primaries = Hits_primaries / Hits_primaries_noshield    # Primary hits in detector with shielding / Primary hits in detector with no shielding
Transmission_factor_onlyprotons = Total_protons/ Total_hits_noshield  # Protons (prim+sec) in detector with shielding / Primary protons in detector with no shielding
Reduction_factor_total = 1 - Transmission_factor_total 
Reduction_factor_primaries = 1 - Transmission_factor_primaries 
Secondary_factor_protons = Total_secondaries_protons / Total_hits_noshield
print("transmission factor prims = ", Transmission_factor_primaries)
print("transmission factor total = ", Transmission_factor_total)
print("reduction factor prims = ", Reduction_factor_primaries)
print("reduction factor total = ", Reduction_factor_total)

print("Trans percentage only protons = ", Transmission_factor_onlyprotons*100)

# Model function: asymptotic regression
def model_transmission_percentage_total(x, a, b, c):
    a = 1  # Set a to 1 for the model
    return (a - b/x**c)*100 

# Fit the model to the data
popt, pcov = curve_fit(model_transmission_percentage_total, E_gen_protons, Transmission_factor_onlyprotons*100, p0=[1, 1e4, 5], maxfev=10000)

# Print the optimal parameters
print(f"Fitted parameters: a={popt[0]:.3f}, b={popt[1]:.3f} c={popt[2]:.3f}")
'''
plt.figure(figsize=(10, 7))
plt.scatter(E_gen_protons, Transmission_factor_primaries, color='green', label='Proton simulation data')
plt.xlabel('Proton Energy (MeV)')
plt.ylabel('Transmission factor primaries')
plt.title('Transmission of primary particles through 5mm Aluminium in vacuum')
plt.legend()
plt.grid(True)
'''
x_values = np.linspace(min(E_gen_protons), max(E_gen_protons), 100)

plt.figure(figsize=(10, 7))
plt.scatter(E_gen_protons, Transmission_factor_onlyprotons*100, color='green', label='Proton simulation data')
plt.plot(x_values, model_transmission_percentage_total(x_values, *popt), color='red', label='Fitted Curve')
plt.xlabel('Proton Energy (MeV)', fontsize=15)
plt.ylabel('Transmission percentage protons [%]', fontsize=15)
plt.title('Transmission percentage of protons through 5mm Aluminium in vacuum', fontsize=16)
plt.legend(fontsize=14)
plt.grid(True)
ax = plt.gca()  # Get the current axes
ax.tick_params(axis='both', which='major', labelsize=12)


x_values_validation = E_gen_protons
y_values_validation = model_transmission_percentage_total(x_values_validation, *popt)
y_values_clipped = np.maximum(y_values_validation, 0)

print(y_values_clipped)

percentage_error = ((y_values_clipped - Transmission_factor_onlyprotons*100) / (Transmission_factor_onlyprotons*100)) * 100

absolute_error = np.abs(y_values_clipped - Transmission_factor_onlyprotons*100)
print(f"Absolute error: {absolute_error}")

print(f"Percentage error: {percentage_error}")

mae = np.mean(np.abs(y_values_clipped-Transmission_factor_onlyprotons*100))

print(f"Mean Absolute Error: {mae:.2f}%")

Test = model_transmission_percentage_total(32.095, *popt)
print(Test)
'''

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
coeffs = np.polyfit(E_gen_protons, Reduction_factor_total, deg=4)
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
plt.scatter(E_gen_protons, Reduction_factor_total, color='red', label='Data points')
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

'''
plt.show()