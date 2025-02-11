import Bethe_bloch_helperfunctions as bb
import numpy as np
import matplotlib.pyplot as plt
from SRIM_data import SRIM_data_Aluminium
from SRIM_data import SRIM_data_Silicon
from SRIM_data import SRIM_data_Air


SRIM_data_Aluminium = np.array(SRIM_data_Aluminium)
SRIM_LET_data_Aluminium = SRIM_data_Aluminium[78:144, 1]  #from 1MeV to 300MeV 
SRIM_Ekin_data_Aluminium_MeV = SRIM_data_Aluminium[78:144, 0]/1e3  #from 1MeV to 300MeV in keV to MeV (so 1000 to 300000)


SRIM_data_Silicon = np.array(SRIM_data_Silicon)
SRIM_LET_data_Silicon = SRIM_data_Silicon[78:144, 1]  #from 1MeV to 300MeV 
SRIM_Ekin_data_Silicon_MeV = SRIM_data_Silicon[78:144, 0]/1e3  #from 1MeV to 300MeV in keV to MeV (so 1000 to 300000)





    
''' 
    # Constants shielding material
    # Constants Aluminium
    material_constants = get_material_constants(material)
    K = 0.307075 # MeV cm^2/g
    Z = material_constants["Z"]  #Atomic number of target particle
    A = material_constants["A"]  #Atomic mass of target particle in u
    rho = material_constants["rho"] #Density of target particle in g/cm^3
    if material == "Air":
        I = 85.7e-6 #MeV from https://physics.nist.gov/cgi-bin/Star/compos.pl?refer=ap&matno=104 
    else:
        I = bb.Ionisation_constant(Z)
    z = 1   #Charge of incident particle in terms of e
    me_c2 = 0.511 #in MeV = me*c^2
    m_p = 1.67e-27 #Mass of proton in kg
    m_e = 9.10938e-31*1e3 #Mass of electron in g


Ekin_values = np.zeros(len(LET_list))
LET_values = np.zeros(len(LET_list))
Ekin_values_MeV = np.zeros(len(LET_list))
for i in range(len(LET_list)):
    Ekin_values[i], LET_values[i] = bb.calculate_kinetic_energy_backward(LET_list[i], distance=5000, dx=1)
    Ekin_values_MeV[i] = [round(value/1e3, 3) for value in Ekin_values[i]] #keV to MeV
print(Ekin_values_MeV[0])
'''

'''
plt.figure(0)
LET_detector = 4.2 #From Geant4 simulation 10kProton35_5MeV_Al_Slab_V3_fStopandKillLET (project_test) in keV/um
Ekin_start = bb.kinetic_energy(bb.beta_from_LET_value(LET_detector, "Silicon")) #in keV
print("The beginning Ekin = ", Ekin_start/1e3, "MeV")
Ekin_before_silicon, LET_before_silicon, distance_before_silicon = bb.calculate_kinetic_energy_backward(Ekin_start, "Silicon", 1, 1, 2e6+1)     
print("Kinetic energy at face detector =", Ekin_before_silicon[-1]/1e3, "MeV")
#print(LET_before_silicon[-1])



Ekin_air_14_2cm, LET_air_14_2cm, distance_air_14_2cm = bb.calculate_kinetic_energy_backward(Ekin_before_silicon[-1], "Air", 142000, 10, distance_before_silicon[-1]*1e4) #14.2cm
print("Kinetic energy at backface Aluminium = ", Ekin_air_14_2cm[-1]/1e3, "MeV")
#print(LET_air_14_2cm[-1])



Ekin_aluminium_2_04mm, LET_aluminium_2_04mm, distance_aluminium_2_04mm = bb.calculate_kinetic_energy_backward(Ekin_air_14_2cm[-1], "Aluminium", 2040, 1, distance_air_14_2cm[-1]*1e4)
#print(LET_aluminium_2_04mm[-1])


Ekin_air_1_85596m, LET_air_1_85596m, distance_air_1_85596m =  bb.calculate_kinetic_energy_backward(Ekin_aluminium_2_04mm[-1], "Air", 1855960, 10, distance_aluminium_2_04mm[-1]*1e4) #1.85596m
print("Kinetic energy at generator = ", Ekin_air_1_85596m[-1]/1e3, "MeV")
#print(LET_air_1_85596m[-1])


# Input values to display
input_values = {"LET at detector [keV/um]": LET_detector, "Ekin at face detector [MeV]": Ekin_before_silicon[-1]/1e3, "Ekin at generator [MeV]": Ekin_air_1_85596m[-1]/1e3}
# Add input values to the legend
input_text = '\n'.join([f"{key}: {value:.2f}" for key, value in input_values.items()])
plt.plot([], [], ' ', label=input_text)  # Invisible line with text in legend

#Plot the calculated values
plt.plot(distance_before_silicon, Ekin_before_silicon, label='Silicon 1um')
plt.plot(distance_air_14_2cm, Ekin_air_14_2cm, label='Air 14.2cm')
plt.plot(distance_aluminium_2_04mm, Ekin_aluminium_2_04mm, label='Aluminium 2.04mm')
plt.plot(distance_air_1_85596m, Ekin_air_1_85596m, label='Air 1.85596m')
plt.xlabel("Distance from generator (cm)")
plt.ylabel("Kinetic Energy (keV)")
plt.title("Backwards calculation of kinetic energy with Al (2.04mm) slab")
plt.legend()


plt.figure(1)
#Calculate start values from LET input from detector
LET_detector_no_slab = 3.2 #From Geant4 simulation 10kProton35_5MeV_Al_Slab_V3_fStopandKillLET (project_test) in keV/um
Ekin_start_no_slab = bb.kinetic_energy(bb.beta_from_LET_value(LET_detector_no_slab, "Silicon")) #in keV
print("The Ekin in the detector with no slab = ", Ekin_start_no_slab/1e3, "MeV")

#Calculate the kinetic energy and LET backwards
Ekin_before_silicon_no_slab, LET_before_silicon_no_slab, distance_before_silicon_no_slab = bb.calculate_kinetic_energy_backward(Ekin_start_no_slab, "Silicon", 1, 1, 2e6+1)
print("Kinetic energy at face detector with no slab =", Ekin_before_silicon_no_slab[-1]/1e3, "MeV")

Ekin_air_2m_no_slab, LET_air_2m_no_slab, distance_air_2m_no_slab =  bb.calculate_kinetic_energy_backward(Ekin_before_silicon_no_slab[-1], "Air", 2e6, 10, distance_before_silicon_no_slab[-1]*1e4) #2m
print("Kinetic energy at generator with no slab = ", Ekin_air_2m_no_slab[-1]/1e3, "MeV")

# Input values to display
input_values = {"LET at detector [keV/um]": LET_detector_no_slab, "Ekin at face detector [MeV]": Ekin_before_silicon_no_slab[-1]/1e3, "Ekin at generator [MeV]": Ekin_air_2m_no_slab[-1]/1e3}
# Add input values to the legend
input_text = '\n'.join([f"{key}: {value:.2f}" for key, value in input_values.items()])
plt.plot([], [], ' ', label=input_text)  # Invisible line with text in legend

# Plot the calculated values
plt.plot(distance_before_silicon_no_slab, Ekin_before_silicon_no_slab, label='Silicon 1um')
plt.plot(distance_air_2m_no_slab, Ekin_air_2m_no_slab,  label='Air 2m')
plt.legend()
plt.xlabel("Distance from generator (cm)")
plt.ylabel("Kinetic Energy (keV)")
plt.title("Backwards calculation of kinetic energy with no slab")

plt.figure(2)
#Test with Geant4 simulation data
LET_list = [37.76, 4.6684, 3.05, 1.446, 0.866] #keV/um --Data from Geant4 simulation
beta_solution = bb.beta_from_LET_list(LET_list, "Silicon") # beta from LET
Ekin_list = bb.kinetic_energy(beta_solution) # kinetic energy at detector


Ekin_values_0, LET_values_0, distance_values_0 = bb.calculate_kinetic_energy_backward(Ekin_list[0], "Aluminium", 5000, dx=1, distance_from_generator=5000)
Ekin_values_MeV_0 = [round(value/1e3, 3) for value in Ekin_values_0] #keV to MeV
print(Ekin_values_MeV_0[-1])
Ekin_values_1, LET_values_1, distance_values_1 = bb.calculate_kinetic_energy_backward(Ekin_list[1], "Aluminium", 5000, dx=1, distance_from_generator=5000)
Ekin_values_MeV_1 = [round(value/1e3, 3) for value in Ekin_values_1] #keV to MeV
print(Ekin_values_MeV_1[-1])
Ekin_values_2, LET_values_2, distance_values_2 = bb.calculate_kinetic_energy_backward(Ekin_list[2], "Aluminium", 5000, dx=1, distance_from_generator=5000)
Ekin_values_MeV_2 = [round(value/1e3, 3) for value in Ekin_values_2] #keV to MeV
print(Ekin_values_MeV_2[-1])
Ekin_values_3, LET_values_3, distance_values_3 = bb.calculate_kinetic_energy_backward(Ekin_list[3], "Aluminium", 5000, dx=1, distance_from_generator=5000)
Ekin_values_MeV_3 = [round(value/1e3, 3) for value in Ekin_values_3] #keV to MeV
print(Ekin_values_MeV_3[-1])
Ekin_values_4, LET_values_4, distance_values_4 = bb.calculate_kinetic_energy_backward(Ekin_list[4], "Aluminium", 5000, dx=1, distance_from_generator=5000)
Ekin_values_MeV_4 = [round(value/1e3, 3) for value in Ekin_values_4] #keV to MeV
print(Ekin_values_MeV_4[-1])

plt.plot(distance_values_0, Ekin_values_MeV_0, label='Ekin @generator= 32.5MeV')
plt.plot(distance_values_1, Ekin_values_MeV_1, label='Ekin @generator= 40.0MeV')
plt.plot(distance_values_2, Ekin_values_MeV_2, label='Ekin @generator= 50.0MeV')
plt.plot(distance_values_3, Ekin_values_MeV_3, label='Ekin @generator= 100.0MeV')
plt.plot(distance_values_4, Ekin_values_MeV_4, label='Ekin @generator= 200.0MeV')
plt.legend(loc="upper left", bbox_to_anchor=(0.01, 0.9))
plt.xlabel("Distance from generator (cm)")
plt.ylabel("Kinetic Energy (keV)")
plt.title("Backwards calculation of kinetic energy with Al slab 5mm")
'''

'''
LET_Cyklotron_no_slab = 3.93 #From Cyklotron experiment 35MeV+2m air = 31MeV detector (DPE_CP_1_1_0_dec2023_251_1s)   in keV/um
Cyklotron_correction_factor = 1.0
beta_solution_Cyklotron_no_slab = bb.beta_from_LET_value(LET_Cyklotron_no_slab*Cyklotron_correction_factor, "Silicon") # beta from LET
Ekin_Cyklotron_no_slab = bb.kinetic_energy(beta_solution_Cyklotron_no_slab) # kinetic energy at detector


Ekin_values_Cyklotron_no_slab, LET_values_Cyklotron_no_slab, distance_values_Cyklotron_no_slab = bb.calculate_kinetic_energy_backward(Ekin_Cyklotron_no_slab, "Silicon", 1, dx=1, distance_from_generator=2e6+1)
print("Kinetic energy of Cyklotron 60deg at face detector with no slab =", Ekin_values_Cyklotron_no_slab[-1]/1e3, "MeV")

Ekin_air_2m_Cyklotron_no_slab, LET_air_2m_Cyklotron_no_slab, distance_air_2m_Cyklotron_no_slab =  bb.calculate_kinetic_energy_backward(Ekin_values_Cyklotron_no_slab[-1], "Air", 2e6, 10, distance_values_Cyklotron_no_slab[-1]*1e4) #2m
print("Kinetic energy of Cyklotron 60deg at generator with no slab = ", Ekin_air_2m_Cyklotron_no_slab[-1]/1e3, "MeV")

plt.figure(3)
plt.plot(distance_values_Cyklotron_no_slab, Ekin_values_Cyklotron_no_slab, label='Silicon 1um')
plt.plot(distance_air_2m_Cyklotron_no_slab, Ekin_air_2m_Cyklotron_no_slab,  label='Air 2m')
plt.legend()
plt.xlabel("Distance from generator (cm)")
plt.ylabel("Kinetic Energy (keV)")
plt.title("Backwards calculation of kinetic energy of Cyklotron 60deg with no slab")
'''

LET_HE_Krakow = 5.26 #From HE_Krakow experiment 35MeV+2m air = 31MeV detector (DPE_CP_1_1_0_dec2023_251_1s)   in keV/um
HE_Krakow_correction_factor = 1.0
beta_solution_HE_Krakow_no_slab = bb.beta_from_LET_value(LET_HE_Krakow*HE_Krakow_correction_factor, "Silicon") # beta from LET
Ekin_HE_Krakow_no_slab = bb.kinetic_energy(beta_solution_HE_Krakow_no_slab) # kinetic energy at detector


Ekin_values_HE_Krakow_no_slab, LET_values_HE_Krakow_no_slab, distance_values_HE_Krakow_no_slab = bb.calculate_kinetic_energy_backward(Ekin_HE_Krakow_no_slab, "Silicon", 1, dx=1, distance_from_generator=2e6+1)
print("Kinetic energy of HE_Krakow 60deg at face detector with no slab =", Ekin_values_HE_Krakow_no_slab[-1]/1e3, "MeV")

Ekin_air_2m_HE_Krakow_no_slab, LET_air_2m_HE_Krakow_no_slab, distance_air_2m_HE_Krakow_no_slab =  bb.calculate_kinetic_energy_backward(Ekin_values_HE_Krakow_no_slab[-1], "Air", 2e6, 10, distance_values_HE_Krakow_no_slab[-1]*1e4) #2m
print("Kinetic energy of HE_Krakow 60deg at generator with no slab = ", Ekin_air_2m_HE_Krakow_no_slab[-1]/1e3, "MeV")

plt.figure(3)
plt.plot(distance_values_HE_Krakow_no_slab, Ekin_values_HE_Krakow_no_slab, label='Silicon 1um')
plt.plot(distance_air_2m_HE_Krakow_no_slab, Ekin_air_2m_HE_Krakow_no_slab,  label='Air 2m')
plt.legend()
plt.xlabel("Distance from generator (cm)")
plt.ylabel("Kinetic Energy (keV)")
plt.title("Backwards calculation of kinetic energy of Cyklotron 60deg with no slab")

plt.show()

# Okay so with no slab, and Ekin at the generator is 35.5MeV --> avg Ekin in detector =32.2MeV --> LET = 3.9 keV/um from Geant4 simulation 10kProton35_5MeV_NoSlab_V2 (project_test)



