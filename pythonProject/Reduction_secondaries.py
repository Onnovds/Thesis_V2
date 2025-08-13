import numpy as np
import matplotlib.pyplot as plt
from scipy.optimize import curve_fit
from numpy.polynomial.polynomial import Polynomial

# Data protons Omnidirectional source
E_gen_protons = np.array([31.7, 32.5, 35, 40, 50, 100, 200])  # Generator energy of protons in MeV
Total_hits_detector = np.array([1565, 2680, 14937, 33373, 47993, 48412, 48664]) #Total hits in detector after shielding
Total_hits_noshield = np.array([46258, 46271, 46267, 46247, 46233, 46276, 46227])  #Total hits in detector with no shielding (100k generated)
Total_secondaries_noshield = np.array([19, 19, 19, 13, 13, 21, 19]) #Total secondaries in detector with no shielding
Total_secondaries_protons = np.array([1564, 1633, 1967, 2497, 2963, 2920, 3165]) #Total secondaries in detector with shielding

Secondary_proportion_protons = Total_secondaries_protons / Total_hits_detector

print(Secondary_proportion_protons)