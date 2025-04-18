import numpy as np
import matplotlib.pyplot as plt
from scipy.optimize import fsolve
from scipy.interpolate import interp1d
from numpy.polynomial.polynomial import Polynomial
#from Bethe_bloch_constants import K, z, m_p, m_e, me_c2

# Constants
K = 0.307075 # MeV cm^2/g
z = 1   #Charge of incident particle in terms of e
me_c2 = 0.511 #in MeV = me*c^2
m_p = 1.67e-27 #Mass of proton in kg
#mp_c2 = 938.272 #in MeV/c^2
m_e = 9.10938e-31*1e3 #Mass of electron in g

def get_material_constants(material):
    materials = {
        "Aluminium": {"Z": 13, "A": 26.98, "rho": 2.70, "fit_factor": 1.61, "fit_factor_air": 1.0},
        "Silicon": {"Z": 14, "A": 28.085, "rho": 2.33, "fit_factor": 1.58, "fit_factor_air": 1.0},
        "Air": {"Z": 8.2, "A": 28.96, "rho": 1.20479E-03, "fit_factor": 1.0, "fit_factor_air": 1.83},
       # "Aluminium": {"Z": 13, "A": 26.98, "rho": 2.70, "fit_factor": 1.0, "fit_factor_air": 1.0},
       # "Silicon": {"Z": 14, "A": 28.085, "rho": 2.33, "fit_factor": 1.0, "fit_factor_air": 1.0},
       # "Air": {"Z": 8.2, "A": 28.96, "rho": 1.20479E-03, "fit_factor": 1.0, "fit_factor_air": 1.0},
    }

    if material not in materials:
        raise ValueError(f"Material '{material}' not found. Available materials: {list(materials.keys())}")

    return materials[material]

#Helper functions
def Ionisation_constant(Z):
    if Z == 0:
        print("Atomic number cannot be zero")
        return None
    if Z <13:
        I = (12*Z + 7)/1e6 #eV to MeV
    elif Z >= 13:
        I = (9.76*Z + 58.8*Z**(-0.19))/1e6 #eV to MeV
    return I

def relativistic_factor(beta):
    gamma = 1/np.sqrt(1-beta**2)
    return gamma


def max_Ekin_proton(me, mp, beta):
    s = me/mp/1e3 #me in g, mp in kg
    c = 3e10 #cm/s
    gamma = relativistic_factor(beta)
    Tmax = 2*me*c**2*beta**2*gamma**2/(1+2*gamma*s+s**2) *(6.241509e5)#in CGS to MeV
    return Tmax


def kinetic_energy(beta):   #(gamma-1)*m*c^2 
    # This is to get kinetic energy once we have velocity
    for i in range(len(beta)):
        if beta[i] < 0.01:
            print("ALARM", beta[i], i)     
    m = 1.67e-27 #in kg
    c = 3e8 #in m/s
    Ekin_proton = m*c**2*(1/np.sqrt(1-beta**2)-1)*6.241509e15 #in keV       #(gamma-1)*m*c**2 = total energy - rest energy = Ekin
    #Ekin_proton = m*c**2*(1/np.sqrt(1-beta**2)-1)*6.241509e15 #in MeV
    return Ekin_proton
def kinetic_energy2(beta): #(gamma-1)*m*c^
    # This is to get kinetic energy from single beta
    m = 1.67e-27 #in kg
    c = 3e8 #in m/s
    Ekin_proton = m*c**2*(1/np.sqrt(1-beta**2)-1)*6.241509e15 #in keV       #(gamma-1)*m*c**2 = total energy - rest energy = Ekin
    return Ekin_proton

def beta_from_kinetic_energy(Ekin): #in keV
    # This is to get velocity once we have kinetic energy
    c = 3e8 #in m/s
    mp = 1.67e-27 #in kg
    Ekin = Ekin/6.241509e15 #in keV to J
    beta = np.sqrt(1-1/(1+Ekin/(mp*c**2))**2)
    return beta

def beta_from_LET_list(LET_data, material): #in keV/um
    beta_solution = np.zeros(len(LET_data))
    for i in range(len(LET_data)):
        Corrected_LET_data = np.zeros(len(LET_data))
        material_constants = get_material_constants(material)
        Z = material_constants["Z"]
        A = material_constants["A"]
        rho = material_constants["rho"]
        fit_factor = material_constants["fit_factor"]
        fit_factor_air = material_constants["fit_factor_air"]
        if material == "Air":
            I = 85.7e-6 #MeV from https://physics.nist.gov/cgi-bin/Star/compos.pl?refer=ap&matno=104 
        else:
            I = Ionisation_constant(Z)
        
        '''
        if LET_data[i] < 0.01 or LET_data[i] > 10: #Ignore LET values that are too low or too high --> 
            LET_data[i] = 0 
            beta_solution[i] = 0
            continue
        ''' 

        #Correction_factor = 1.6    #Correction factor for LET values but I don't know for what anymore
        dEdx = round(LET_data[i]*10, 5) #  keV/um to MeV/cm
        func = lambda beta : -dEdx + fit_factor_air*K*Z/A*rho*z**2/beta**2*(0.5*np.log(fit_factor*2*me_c2**2*beta**2*relativistic_factor(beta)**2*max_Ekin_proton(m_e, m_p, beta)/I**2) - beta**2)
        #func = lambda beta : dEdx + K*Z/A*rho*z**2/beta**2*(np.log(2*m_e*beta**2*(relativistic_factor(beta))**2/I) - beta**2)
        #func = lambda beta : dEdx + K*Z/(A*beta**2)*(np.log(2*m_e*c_cm**2*beta**2/(1-beta**2)/I)-beta**2)
        
        # Normalise LET_value
        LET_normalised = LET_data[i] / (rho * Z)
        # Dynamically set beta_initial_guess based on LET and material properties
        # This only partially works --> best to adjust for every material
        if LET_normalised> 0.6:
            beta_initial_guess = 0.03        
        elif LET_normalised> 0.1:  # Use atomic number Z as a threshold for high LET
            beta_initial_guess = 0.07
        elif LET_normalised > 0.05:  # Adjust for mid-range LET
            beta_initial_guess = 0.33
        else:  # Low LET
            beta_initial_guess = 0.55
        beta_solution[i] = fsolve(func, beta_initial_guess).item()
        if beta_solution[i] < 0:
            print("The solution is incorrect as velocity is negative")
            print("The LET value is: ", LET_data[i])
            print("The LET_normalised value is: ", LET_normalised)
    for j in range(len(beta_solution)):         #Check if the solution is correct
        if beta_solution[j] < 0:
            print("The solution is incorrect as velocity is negative")
            print("The beta initial guess is: ", beta_initial_guess)
    return beta_solution

def beta_from_LET_value(LET_value, material): #in keV/um
    material_constants = get_material_constants(material)
    Z = material_constants["Z"]
    A = material_constants["A"]
    rho = material_constants["rho"]
    fit_factor = material_constants["fit_factor"]
    fit_factor_air = material_constants["fit_factor_air"]
    if material == "Air":
        I = 85.7e-6 #MeV from https://physics.nist.gov/cgi-bin/Star/compos.pl?refer=ap&matno=104 
    else:
        I = Ionisation_constant(Z)

    dEdx = round(LET_value*10, 5) #  keV/um to MeV/cm
    func = lambda beta : -dEdx + fit_factor_air*K*Z/A*rho*z**2/beta**2*(0.5*np.log(fit_factor*2*me_c2**2*beta**2*relativistic_factor(beta)**2*max_Ekin_proton(m_e, m_p, beta)/I**2) - beta**2)
    #func = lambda beta : dEdx + K*Z/A*rho*z**2/beta**2*(np.log(2*m_e*beta**2*(relativistic_factor(beta))**2/I) - beta**2)
    #func = lambda beta : dEdx + K*Z/(A*beta**2)*(np.log(2*m_e*c_cm**2*beta**2/(1-beta**2)/I)-beta**2)

    # Normalise LET_value
    LET_normalised = LET_value / (rho * Z)
    #print("LET_normalised=", LET_normalised)
    
    # Dynamically set beta_initial_guess based on LET and material properties
    if LET_normalised> 0.6:
        beta_initial_guess = 0.05
    elif LET_normalised> 0.1:  # Use atomic number Z as a threshold for semi-high LET
        beta_initial_guess = 0.1
    elif LET_normalised > 0.05:  # Adjust for mid-range LET
        beta_initial_guess = 0.33
    else:  # Low LET
        beta_initial_guess = 0.55
    beta_solution = fsolve(func, beta_initial_guess).item()
    #Check if the solution is correct
    if beta_solution < 0:
        print("The solution is incorrect as velocity is negative")
        print("The LET value is: ", LET_value)
        print("The LET_normalised value is: ", LET_normalised)
    return beta_solution

def percentage_error(SRIM_data, calculated_data):
    error = np.zeros(len(SRIM_data))
    for i in range(len(SRIM_data)):
        error[i] = (SRIM_data[i] - calculated_data[i])/SRIM_data[i]*100
    return error

def LET_from_beta(beta, material):
    K = 0.307075 # MeV cm^2/g
    z = 1
    m_p = 1.67e-27 #Mass of proton in kg
    m_e = 9.10938e-31*1e3 #Mass of electron in g
    me_c2 = 0.511 #in MeV = me*c^2
    material_constants = get_material_constants(material)
    Z = material_constants["Z"]
    A = material_constants["A"]
    rho = material_constants["rho"]
    fit_factor = material_constants["fit_factor"]
    fit_factor_air = material_constants["fit_factor_air"]
    if material == "Air":
        I = 85.7e-6 #MeV from https://physics.nist.gov/cgi-bin/Star/compos.pl?refer=ap&matno=104 
    else:
        I = Ionisation_constant(Z)

    dEdx = fit_factor_air*K*Z/A*rho*z**2/beta**2*(0.5*np.log(fit_factor*2*me_c2**2*beta**2*relativistic_factor(beta)**2*max_Ekin_proton(m_e, m_p, beta)/I**2) - beta**2)
    LET = dEdx/10 #MeV/cm to keV/um
    return LET


def calculate_kinetic_energy_backward(Ekin_start, material, width, dx, distance_from_generator):
    """
    Calculate the kinetic energy and LET backward from a known starting kinetic energy.

    Parameters:
    Ekin_start (float): The kinetic energy of the particle at the detector (in keV).
    material (string): The material through which the particle is traveling the detector (e.g. "Aluminium").
    width (float): The width of the medium to calculate backward (in μm).
    dx (float): The step size for width of the medium (in μm).
    distance_from_generator (float): The distance from the generator to the detector (in μm).

    Returns:
    tuple: The backwards calculated kinetic energy and corresponding LET at the "start" of the medium (in keV and keV/μm).
    """

    if Ekin_start <= 0:
        Ekin_values = [0]
        LET_values = [0]
        distance_values = [0]
        return Ekin_values, LET_values, distance_values
    

    # Convert the total width to the number of steps
    steps = int(width / dx)

    # Initialize the current kinetic energy and LET
    beta_solution_start = beta_from_kinetic_energy(Ekin_start) # beta from LET
    Ekin = Ekin_start
    LET_current = LET_from_beta(beta_solution_start, material) #in keV/um
    LET_start = LET_current

    # Initialize arrays to store kinetic energy and LET values
    Ekin_values = [Ekin_start]
    LET_values = [LET_start]
     
    distance_values = [distance_from_generator/1e4] #in cm
    
    for _ in range(steps):
        # Update the kinetic energy by stepping backward
        Ekin += LET_current * dx  #in keV
        beta_current = beta_from_kinetic_energy(Ekin) #function uses keV
        distance_from_generator -= dx #Subtract from the total distance

        # Find the new LET corresponding to the updated kinetic energy
        LET_current = LET_from_beta(beta_current, material) #in MeV/cm
        # Store the current values in the arrays
        Ekin_values.append(Ekin)
        LET_values.append(LET_current)
        distance_values.append(distance_from_generator/1e4) #in cm
        #print("The distance from generator is now: ", distance_from_generator, "cm")

    Ekin_values = [float(value) for value in Ekin_values]
    LET_values = [float(value) for value in LET_values]
    return Ekin_values, LET_values, distance_values




#------------------------------------Vectorised version of the function-------------------------------------


# Precompute beta values for LET
# Define range
x_min, x_max = 0.419, 59.28
n_points = 1000  # Total points

# Exponential scaling
scaled_values = np.linspace(0, np.log1p(x_max - x_min), n_points)  # Log-space
LET_range_Silicon_log = np.expm1(scaled_values) + x_min  # Reverse log transform --> log scale as we want more points at lower LET values
#LET_range_Silicon = np.linspace(0.419, 59.28 , 1000)  # Define a range of LET values --- 0.419keV/um --> 1GeV proton in Silicon, 59.28keV/um --> ~0.5MeV proton in Silicon
beta_solutions_Silicon = np.array([beta_from_LET_value(LET, "Silicon") for LET in LET_range_Silicon_log])
beta_interp_Silicon = interp1d(LET_range_Silicon_log, beta_solutions_Silicon, kind='cubic', fill_value="extrapolate")

def beta_from_LET_list_Silicon_vectorised(LET_data):
    return beta_interp_Silicon(LET_data)  # Interpolated instead of solving each time

LET_range_Air = np.linspace(2.352e-04, 0.01411, 1000)  # Define a range of LET values --- 2.352e-4keV/um --> 1GeV proton in Air, 0.01411keV/um --> 1keV proton in Air
beta_solutions_Air = np.array([beta_from_LET_value(LET, "Air") for LET in LET_range_Air])
beta_interp_Air = interp1d(LET_range_Air, beta_solutions_Air, kind='cubic', fill_value="extrapolate")

def beta_from_LET_list_Air_vectorised(LET_data):
    return beta_interp_Air(LET_data)  # Interpolated instead of solving each time



def calculate_kinetic_energy_backward_optimised(Ekin_start_array, material, width, dx, distance_from_generator):

    steps = int(width / dx)
    

    # Create matrix where each row is a data point and columns are the steps
    Ekin_values = np.zeros((len(Ekin_start_array), steps + 1))
    LET_values = np.zeros((len(Ekin_start_array), steps + 1))

    # Set the initial conditions for all values at the same time
    Ekin_values[:, 0] = Ekin_start_array
    beta_values = beta_from_kinetic_energy(Ekin_start_array)  # Initial beta
    LET_values[:, 0] = LET_from_beta(beta_values, material)  # Get LET

    # Create vector of distances
    distances = np.linspace(distance_from_generator, distance_from_generator - width, steps) / 1e4

    # Vectorised kinetic energy update
    for i in range(1, steps + 1):
        Ekin_values[:, i] = Ekin_values[:, i - 1] + LET_values[:, i - 1] * dx
        beta_values = beta_from_kinetic_energy(Ekin_values[:, i])
        LET_values[:, i] = LET_from_beta(beta_values, material)

    return Ekin_values, LET_values, distances

def calculate_kinetic_energy_backward_optimised2(Ekin_start_array, material, width, dx, distance_from_generator):
    """
    Optimised backward kinetic energy calculation (avoids huge memory usage).

    Parameters:
    - Ekin_start_array (np.array): Kinetic energy at the detector (keV).
    - material (str): Material medium (e.g., "Air").
    - width (float): Total distance traveled (μm).
    - dx (float): Step size (μm).
    - distance_from_generator (float): Initial distance from source (μm).

    Returns:
    - Ekin_final: Array of kinetic energy at the source.
    """

    num_steps = int(width / dx)

    # Create array for step distances (shape: (num_steps,))
    distances = np.linspace(distance_from_generator, distance_from_generator - width, num_steps)

    # Initialise kinetic energy values (shape: (num_particles,))
    Ekin_values = np.copy(Ekin_start_array)

    for _ in range(num_steps):
        beta_values = beta_from_kinetic_energy(Ekin_values)  # Vectorised beta computation
        LET_values = LET_from_beta(beta_values, material)  # Vectorised LET computation
        Ekin_values += LET_values * dx  # Vectorised energy update

    return Ekin_values  # Only return final kinetic energy per particle

#------------------------------------------------------------------------------------------------------------


# --------------------------------------- Electron formulas ---------------------------------------

def LET_to_Ekin_function(LET_column, Ekin_column):

    degree = 4 # Polynomial degree for fitting --> adjust if necessary
    # Convert E_kin to log-space for better fitting
    log_Ekin = np.log10(Ekin_column)
    
    # Fit polynomial in log-space
    coefs = np.polyfit(LET_column, log_Ekin, deg=degree)  
    LET_to_logEkin_poly = np.poly1d(coefs)

    # Function to predict Ekin from LET
    def predict_Ekin(LET_values):
        return 10 ** LET_to_logEkin_poly(LET_values)  # Convert back from log-space
    
    return predict_Ekin

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from scipy.interpolate import interp1d

# Load data
Electron_in_Aluminium_1_5plus = np.array(pd.read_csv("/home/onno/satellite_test/pythonProject/Electrons_in_Aluminium_10keV_1GeV_ESTAR.txt", sep='\s+', header=2))[34:, :]
Electron_in_Aluminium_1_5min= np.array(pd.read_csv("/home/onno/satellite_test/pythonProject/Electrons_in_Aluminium_10keV_1GeV_ESTAR.txt", sep='\s+', header=2))[:34, :]
print(Electron_in_Aluminium_1_5min[-1])

# Extract LET (converted to keV/µm) and kinetic energy
LET_data_1_5plus = Electron_in_Aluminium_1_5plus[:,3] * 2.7 / 10
Ekin_data_1_5plus = Electron_in_Aluminium_1_5plus[:,0]

LET_data_1_5min = Electron_in_Aluminium_1_5min[:,3] * 2.7 / 10
Ekin_data_1_5min = Electron_in_Aluminium_1_5min[:,0]

print(LET_data_1_5min)

# Create interpolation function
interp_function_1_5plus = interp1d(LET_data_1_5plus, Ekin_data_1_5plus, kind='linear', fill_value="extrapolate")  
interp_function_1_5min = interp1d(LET_data_1_5min, Ekin_data_1_5min, kind='linear', fill_value="extrapolate")

# Function to predict kinetic energy from LET using interpolation
def predict_Ekin_from_LET_1_5plus(LET_values):
    return interp_function_1_5plus(LET_values)
def predict_Ekin_from_LET_1_5min(LET_values):
    return interp_function_1_5min(LET_values)

# Generate LET range for predictions
LET_range = np.linspace(0.35, 10, 500)
LET_range_min = np.linspace(0.35, 4.5, 500)
Ekin_predicted_1_5plus = predict_Ekin_from_LET_1_5plus(LET_range)
Ekin_predicted_1_5min = predict_Ekin_from_LET_1_5min(LET_range_min)

# Plot
plt.scatter(Ekin_data_1_5plus, LET_data_1_5plus, color='red', label="Original Data", alpha=0.7)
plt.scatter(Ekin_data_1_5min, LET_data_1_5min, color='red', label="Original Data", alpha=0.7)
plt.plot(Ekin_predicted_1_5min, LET_range_min, label="Linear Interpolation", color='green', linewidth=2)
plt.plot(Ekin_predicted_1_5plus, LET_range, label="Linear Interpolation", color='blue', linewidth=2)
plt.xlabel("Kinetic Energy (E_kin) [keV]")
plt.ylabel("Stopping Power (LET) [keV/µm]")
plt.xscale('log')
plt.legend()
plt.show()




def kinetic_energy_electrons(beta):   #(gamma-1)*m*c^2 
    # This is to get kinetic energy once we have velocity
    for i in range(len(beta)):
        if beta[i] < 0.01:
            print("ALARM", beta[i], i)     
    m = 9.10938e-31 #in kg
    c = 3e8 #in m/s
    Ekin_electron = m*c**2*(1/np.sqrt(1-beta**2)-1)*6.241509e15 #in keV       #(gamma-1)*m*c**2 = total energy - rest energy = Ekin
    #Ekin_proton = m*c**2*(1/np.sqrt(1-beta**2)-1)*6.241509e15 #in MeV
    return Ekin_electron
