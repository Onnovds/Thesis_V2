from SRIM_data import SRIM_data_Silicon
from SRIM_data import SRIM_data_Air
from SRIM_data import SRIM_data_Aluminium
import matplotlib.pyplot as plt


ion_energy = SRIM_data_Silicon[:, 0]
let_elec = SRIM_data_Silicon[:, 1]
let_nuclear = SRIM_data_Silicon[:, 2]

# Plot the data
plt.figure(figsize=(10, 6))
plt.plot(ion_energy, let_elec, label='LET Elec.', color='b')
#plt.plot(ion_energy, let_nuclear, label='LET Nuclear', color='r')
plt.xlabel('Ion Energy (keV)')
plt.ylabel('LET (keV/um)')
plt.title('Ion Energy vs LET')
#plt.legend()
#plt.xscale('log')
plt.yscale('log')
plt.grid(True)
plt.show()