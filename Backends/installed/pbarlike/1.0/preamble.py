# %% Imports and global variables
print("\033[32m Loading python backend - pbarlike 1.0")

import os
os.environ["TF_CPP_MIN_LOG_LEVEL"] = "3"
import gc
import numpy as np
from tensorflow import keras
from iminuit import Minuit
import sys
print("\033[32m Imported required python modules - gc, numpy, tensorflow, iminuit.Minuit")

script_directory = os.path.dirname(os.path.realpath(__file__))
m_p = 0.9382720881604903  # Mass of proton in GeV (938.2720881604903 MeV)


# %% Loading and preparing data
# # Loading AMS Data for antiproton flux:
# ## pbar_ams02_7y.txt
# - Column 1: Rigidity [GV] from 
# - Column 2: Rigidity [GV] to
# - Column 3: Number of antiprotons
# - Column 4: pbar/p ratio
# - Column 5: sigma stat
# - Column 6: sigma sys (on ratio)              !!not given
# - Column 7: sigma sys (on folded acceptance)  !!not given
# - Column 8: sigma sys (on Rigidity scale)     !!not given 
# - Column 9: sigma sys (total)
# - Column10: Factor (for columns 3-9)
# - Column11: Antiproton flux in m-2 sr-1 s-1 GV-1
# - Column12: sigma stat
# - Column13: sigma sys
# - Column14: Factor (for columns 11-13)

# # Preparing data
# ## Converting R to kinetic energy per nucleon: 
# $$ E_k / A = \sqrt{(RZ/A)^2 + m^2} $$
# 
# ## Converting flux in R to flux in E :
# $$ \frac{d \phi}{d R} \frac{d R}{d (E_k/A)} = \frac{d \phi}{d R} \frac{1}{R} (\frac{A}{Z})^2 \sqrt{( \frac{RA}{Z})^2 + m^2} $$

# %%

# Loading 7 year ams02_data which has columns as above :
ams_data = np.genfromtxt(script_directory+'/dependencies/pbar_ams02_7y.txt')
R_ams = (ams_data[:,0]+ams_data[:,1])/2 
phi_in_R = ams_data[:,10] * ams_data[:,13]
error_R_ams = np.sqrt(ams_data[:,11]**2+ams_data[:,12]**2)*ams_data[:,13]

# R - array with rigidity values in GV; outputs array of E_k in GeV
def R_to_Ekin(R,z = -1, A = 1, m = m_p ):
    Z = np.abs(z)
    return np.sqrt(R**2 * (Z/A)**2 + m**2) - m

# flux_in_R - array of flux values measured at different rigidities in m-2 sr-1 s-1 GV-1; outputs array of flux values measured at
# different energies in m-2 sr-1 s-1 GeV-1
def flux_in_Ekin(flux_in_R,R,z=-1,A=1,m=m_p):
    Z = np.abs(z)
    return flux_in_R /R * (A/Z)**2 * np.sqrt((R*Z/A)**2 + m**2)

# E_ams -(58,) array of KE per nucleon values at which AMS02 flux measurements are recorded for pbar
E_ams = np.array(R_to_Ekin(R_ams))
# phi_ams - (58,) array of flux values measured at E_ams
phi_ams = np.array(flux_in_Ekin(phi_in_R,R_ams))
# Error propagation gives the error of flux in E_k as follows:
# error_ams - (58,) array of error values of flux values measured at E_ams
error_ams = np.array(flux_in_Ekin(error_R_ams,R_ams))

# E_drn - (28,) array of KE per nucleon values at which sNet predicts flux values (same values at which training data are given to the sNet)
E_drn = np.array(np.load(script_directory+'/dependencies/E.npy'))
print("\033[32m Loaded and processed AMS-02 dataset")
