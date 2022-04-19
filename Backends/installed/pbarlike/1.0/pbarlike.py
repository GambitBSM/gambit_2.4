#%%

from preamble import *
from DRN_interface import *
from solar_mod import *
print("\033[32m Loaded required custom python modules - DRN_interface, solar_mod")

#%% Applying solar modulation; Adding DM and CR contributions to antiproton flux

def br_fr(input):
    bf = np.zeros((1,8))
    channels = ["q qbar", "c cbar", "b bbar", "t tbar", "W- W+", "Z0 Z0", "g g", "h h"]
    for idx,val in enumerate(channels):
        if val in input :
            bf[0,idx] = input[val] 
    return bf
    

def pbar_flux(DM_masses,sigma_v,br_fr):
    propagation_parameters = usual_theta_prop(1)
    phi_DM_LIS = DM_pbar_flux(DM_masses,br_fr,sigma_v, propagation_parameters)
    phi_CR_LIS      = CR_pbar_sim(propagation_parameters)
    phi_LIS         = phi_CR_LIS + phi_DM_LIS

    V_profiled,A_profiled = nuisance_estimation(phi_LIS, E_drn)
    phi_DMCR              = solar_mod(phi_LIS*A_profiled, E_drn, V_profiled )
    return phi_DMCR[0]

print("\033[37m     Loaded pbarlike 1.0")