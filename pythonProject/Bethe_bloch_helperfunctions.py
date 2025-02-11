import numpy as np
import matplotlib.pyplot as plt
from scipy.optimize import fsolve
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
    m = 1.67e-27 #in kg
    c = 3e8 #in m/s
    Ekin_proton = m*c**2*(1/np.sqrt(1-beta**2)-1)*6.241509e15 #in keV       #(gamma-1)*m*c**2 = total energy - rest energy = Ekin
    #Ekin_proton = m*c**2*(1/np.sqrt(1-beta**2)-1)*6.241509e15 #in MeV
    return Ekin_proton
def kinetic_energy2(beta, m): #1/2mv^2
    # This is to get kinetic energy once we have velocity
    #Ekin_proton = m*c**2*(1/np.sqrt(1-beta**2)-1)*6.241509e12 #in MeV       #(gamma-1)*m*c**2 = total energy - rest energy = Ekin
    c = 3e8 #in m/s
    Ekin_proton = 1/2*m*beta**2*c**2*6.241509e12 #in MeV
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
        
        if LET_data[i] < 1 or LET_data[i] > 4: #Ignore LET values that are too low or too high
            LET_data[i] = 0 
            beta_solution[i] = 0
            continue
        
        Correction_factor = 1.6
        dEdx = round(Correction_factor*LET_data[i]*10, 5) #  keV/um to MeV/cm
        func = lambda beta : -dEdx + fit_factor_air*K*Z/A*rho*z**2/beta**2*(0.5*np.log(fit_factor*2*me_c2**2*beta**2*relativistic_factor(beta)**2*max_Ekin_proton(m_e, m_p, beta)/I**2) - beta**2)
        #func = lambda beta : dEdx + K*Z/A*rho*z**2/beta**2*(np.log(2*m_e*beta**2*(relativistic_factor(beta))**2/I) - beta**2)
        #func = lambda beta : dEdx + K*Z/(A*beta**2)*(np.log(2*m_e*c_cm**2*beta**2/(1-beta**2)/I)-beta**2)
        
        # Normalise LET_value
        LET_normalised = LET_data[i] / (rho * Z)
        # Dynamically set beta_initial_guess based on LET and material properties
        if LET_normalised> 0.1:  # Use atomic number Z as a threshold for high LET
            beta_initial_guess = 0.1
        elif LET_normalised > 0.05:  # Adjust for mid-range LET
            beta_initial_guess = 0.33
        else:  # Low LET
            beta_initial_guess = 0.55
        beta_solution[i] = fsolve(func, beta_initial_guess).item()
    for j in range(len(beta_solution)):         #Check if the solution is correct
        if beta_solution[j] < 0:
            print("The solution is incorrect as velocity is negative")
        #print("The beta solution is: ", beta_solution[j])
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
    if LET_normalised> 0.1:  # Use atomic number Z as a threshold for high LET
        beta_initial_guess = 0.1
    elif LET_normalised > 0.05:  # Adjust for mid-range LET
        beta_initial_guess = 0.33
    else:  # Low LET
        beta_initial_guess = 0.55
    beta_solution = fsolve(func, beta_initial_guess).item()
    #Check if the solution is correct
    if beta_solution < 0:
        print("The solution is incorrect as velocity is negative")
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
