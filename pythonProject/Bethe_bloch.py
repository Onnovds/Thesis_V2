import numpy as np
import matplotlib.pyplot as plt
from scipy.optimize import fsolve
from SRIM_data import SRIM_data_Aluminium
from SRIM_data import SRIM_data_Silicon
from SRIM_data import SRIM_data_Air
import Bethe_bloch_helperfunctions as bb


SRIM_data_Aluminium = np.array(SRIM_data_Aluminium)
SRIM_LET_data_Aluminium = SRIM_data_Aluminium[78:144, 1]  #from 1MeV to 300MeV 
SRIM_Ekin_data_Aluminium_MeV = SRIM_data_Aluminium[78:144, 0]/1e3  #from 1MeV to 300MeV in keV to MeV (so 1000 to 300000)


SRIM_data_Silicon = np.array(SRIM_data_Silicon)
SRIM_LET_data_Silicon = SRIM_data_Silicon[78:144, 1]  #from 1MeV to 300MeV 
SRIM_Ekin_data_Silicon_MeV = SRIM_data_Silicon[78:144, 0]/1e3  #from 1MeV to 300MeV in keV to MeV (so 1000 to 300000)

SRIM_data_Air = np.array(SRIM_data_Air)
SRIM_LET_data_Air = SRIM_data_Air[78:144, 1]  #from 1MeV to 300MeV
SRIM_Ekin_data_Air_MeV = SRIM_data_Air[78:144, 0]/1e3  #from 1MeV to 300MeV in keV to MeV (so 1000 to 300000)


beta_solution = bb.beta_from_LET_list(SRIM_LET_data_Air, "Air")
Ekin = bb.kinetic_energy(beta_solution)
error = bb.percentage_error(SRIM_Ekin_data_Air_MeV, Ekin/1e3)
print("The error percentage = ", error)
print("The calculated kinetic energy = ", Ekin/1e3)
print("The calculated beta = ", beta_solution)


plt.plot(SRIM_Ekin_data_Air_MeV, SRIM_LET_data_Air, label="SRIM data")
plt.plot(Ekin/1e3, SRIM_LET_data_Air, 'ro', label="Calculated data")
#plt.plot(Ekin2, SRIM_LET_data_Aluminium, 'yo', label="1/2mv^2")
plt.xlabel("Kinetic Energy (MeV)")
plt.ylabel("LET (keV/um)")
plt.title("LET over kinetic energy Air")
plt.yscale('log')
plt.xscale('log')
plt.legend()
plt.show()







# Function to calculate Zeff using the provided formula
def calculate_Zeff_air(composition):
    # Step 1: Compute total number of electrons
    f = sum(
        comp["fraction"] * comp["Z"] for comp in composition.values()
    )

    # Step 2: Calculate fi * Z^2.94 for each component
    contributions = [
        (comp["fraction"] * comp["Z"] / f) * (comp["Z"] ** 2.94)
        for comp in composition.values()
    ]

    # Step 3: Sum the contributions and take the (1/2.94)-th power
    Zeff = (sum(contributions)) ** (1 / 2.94)
    return Zeff


# Composition of dry air (same as before)
composition = {
    "CO2": {"Z": 6, "A": 13, "fraction": 0.000124},
    "N2": {"Z": 7, "A": 14.007, "fraction": 0.755267},
    "O2": {"Z": 8, "A": 15.999, "fraction": 0.231781},
    "Ar": {"Z": 18, "A": 39.948, "fraction": 0.012827},
}


# Calculate Zeff
Zeff_air = calculate_Zeff_air(composition)
print(Zeff_air)