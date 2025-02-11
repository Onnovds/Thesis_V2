import matplotlib.pyplot as plt

from SRIM_data import SRIM_data_Silicon
from SRIM_data import SRIM_data_Aluminium
from SRIM_data import SRIM_data_Air
import numpy as np
import Bethe_bloch_helperfunctions as bb

LET_detector = 5 #keV/um
i = 0

def get_kinetic_energy_from_let(LET_detector, SRIM_data_Silicon):
    """
    Given a LET value, find the corresponding kinetic energy with linear interpolation.

    Parameters:
    LET_detector (float): The LET value to find the corresponding kinetic energy for.
    SRIM_data_Silicon (np.ndarray): A 2D NumPy array where the first column is kinetic energy and
                                    the second column is LET.

    Returns:
    float: The interpolated kinetic energy corresponding to the provided LET value.
    """
    # Ensure SRIM_data_Silicon is a NumPy array
    SRIM_data_Silicon = np.array(SRIM_data_Silicon)

    # Extract kinetic energy and LET values
    E_kin = SRIM_data_Silicon[:, 0]
    LET_values = SRIM_data_Silicon[:, 1]

    # Traverse through the LET values
    for i in range(1, len(LET_values)):
        # Check if LET_detector falls between the current and previous LET values
        if LET_values[i] <= LET_detector <= LET_values[i - 1]:
            # Perform linear interpolation
            Ekin_start = E_kin[i - 1] + (LET_detector - LET_values[i - 1]) * (E_kin[i] - E_kin[i - 1]) / (
                        LET_values[i] - LET_values[i - 1])
            return Ekin_start

    # Handle edge cases if LET_detector is out of the range
    if LET_detector >= LET_values[0]:
        return E_kin[0]
    elif LET_detector <= LET_values[-1]:
        return E_kin[-1]

    # If no match is found, return None (this should ideally never happen)
    return None

def get_let_from_kinetic_energy(Ekin, SRIM_data_Silicon):
    """
    Given a kinetic energy, find the corresponding LET by interpolating the SRIM data.

    Parameters:
    Ekin (float): The kinetic energy in MeV.
    SRIM_data_Silicon (np.ndarray): A 2D NumPy array where the first column is kinetic energy
                                    and the second column is LET.

    Returns:
    float: Interpolated LET value in keV/μm.
    """
    kinetic_energies = SRIM_data_Silicon[:, 0]
    LET_values = SRIM_data_Silicon[:, 1]

    if Ekin <= kinetic_energies[0]:
        return LET_values[0]
    elif Ekin >= kinetic_energies[-1]:
        return LET_values[-1]
    else:
        return np.interp(Ekin, kinetic_energies, LET_values)



# Now I need to make a step method using Ekin[i+1] = Ekin[i] + dE/dx[i] * dx
# dE/dx from data = keV/micron so let's say dx = 10 microns


def calculate_kinetic_energy_backward(Ekin_start, LET_start, SRIM_data_Silicon, distance=250, dx=1):
    """
    Calculate the kinetic energy and LET backward from a known starting kinetic energy.

    Parameters:
    Ekin_start (float): The kinetic energy of the particle at the detector (in keV).
    LET_start (float): The LET value at the detector (in keV/μm).
    SRIM_data_Silicon (np.ndarray): A 2D NumPy array where the first column is kinetic energy and
                                    the second column is LET.
    distance (float): The distance to calculate backward (in μm).
    dx (float): The step size for distance (in μm).

    Returns:
    tuple: The calculated kinetic energy and corresponding LET before the detector (in keV and keV/μm).
    """
    # Convert the total distance to the number of steps
    steps = int(distance / dx)

    # Initialize the current kinetic energy and LET
    Ekin = Ekin_start
    LET_current = LET_start

    # Initialize arrays to store kinetic energy and LET values
    Ekin_values = [Ekin_start]
    LET_values = [LET_start]

    for _ in range(steps):
        # Update the kinetic energy by stepping backward
        Ekin += LET_current * dx

        # Find the new LET corresponding to the updated kinetic energy
        LET_current = get_let_from_kinetic_energy(Ekin, SRIM_data_Silicon)

        # Store the current values in the arrays
        Ekin_values.append(Ekin)
        LET_values.append(LET_current)

    return Ekin_values, LET_values



Test_Ekin = get_kinetic_energy_from_let(4.3, SRIM_data_Silicon)
print(Test_Ekin)

Ekin_test = get_kinetic_energy_from_let(4.3, SRIM_data_Silicon)
LET_Test = get_let_from_kinetic_energy(Ekin_test, SRIM_data_Silicon)

Ekin_before_silicon, LET_before_silicon = calculate_kinetic_energy_backward(Ekin_test, LET_Test, SRIM_data_Silicon, 1, 1)
plt.plot(Ekin_before_silicon, LET_before_silicon, label='Silicon 1um')

Ekin_air_two, LET_air_two =  calculate_kinetic_energy_backward(Ekin_before_silicon[-1], LET_before_silicon[-1], SRIM_data_Air, 1855960, 10) #Distance = 200 - 14.2 - 0.204
plt.plot(Ekin_air_two, LET_air_two, label='Air 1.85596m')

Ekin_aluminium, LET_aluminium = calculate_kinetic_energy_backward(Ekin_air_two[-1], LET_air_two[-1], SRIM_data_Aluminium, 2040, 1)
plt.plot(Ekin_aluminium, LET_aluminium, label='Aluminium 2.04mm')

Ekin_generator, LET_generator = calculate_kinetic_energy_backward(Ekin_aluminium[-1], LET_aluminium[-1], SRIM_data_Air, 142000, 10)
plt.plot(Ekin_generator, LET_generator, label='Air 14.2cm')
print("Kinetic Energy at generator: ", Ekin_generator[-1]/1000, 'MeV')
plt.legend()
plt.xlabel('E [keV]')
plt.ylabel('LET [keV/um]')
plt.title('LET over kinetic energy aluminium slab 2.04mm')
plt.show()


Ekin_No_slab_start = get_kinetic_energy_from_let(3.322, SRIM_data_Silicon)
LET_No_slab_start = get_let_from_kinetic_energy(Ekin_No_slab_start, SRIM_data_Silicon)

Ekin_No_slab_silicon, LET_No_slab_silicon = calculate_kinetic_energy_backward(Ekin_No_slab_start, LET_No_slab_start, SRIM_data_Silicon, 1, 1)
Ekin_No_slab_generator, LET_No_slab_generator = calculate_kinetic_energy_backward(Ekin_No_slab_silicon[-1], LET_No_slab_silicon[-1], SRIM_data_Air, 2e6, 10)

print("Kinetic Energy at generator: ", Ekin_No_slab_generator[-1]/1000, 'MeV')
plt.plot(Ekin_No_slab_silicon, LET_No_slab_silicon, label='Silicon 1um')
plt.plot(Ekin_No_slab_generator, LET_No_slab_generator, label='Air 2m')
plt.legend()
plt.xlabel('E [keV]')
plt.ylabel('LET [keV/um]')
plt.title('LET over kinetic energy no slab')
plt.show()
