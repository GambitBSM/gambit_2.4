#%% 
from preamble import *
from DRN_interface import *
# from solar_mod import *
from pbarlike import *
import matplotlib.pyplot as plt
#%%
m_DM = np.array([100.])
bf = np.array([0,0,0,0,1,0,0,0])
sv = 3e-26
pp = np.array([1.8, 1.79, 2.405, 2.357, 7.92e+03, 0.37, 2.05e+28, 0.419, 8.84, 0.09, 2.60])
pp_db = np.array([2.34, 2.28, 3.63e+28, -0.66, 0.52, -0.15, 3.83e+3, 0.4, 2.05e+5, 0.21])
pp_mns = np.array([ 2.34276385e+00,  2.27911723e+00,  3.89201119e+28, -6.09889613e-01,
        5.01982466e-01, -1.57519382e-01,  3.98278071e+03,  4.02670382e-01,
        2.54902724e+05,  3.67344838e+00])
pm = "run1"
prevent_extrapolation = False
#%%
# theta_prop,phi_DMCR,phi_CR = pbar_flux(m_DM,sv,bf,propagation_model,pp,marginalization)
# chi2_DMCR,chi2_CR = chi2_array(phi_DMCR,phi_CR)
# del_chi = del_chi2(chi2_DMCR,chi2_CR,theta_prop)

# DRN = DRNet(pm,prevent_extrapolation)
# DRN.preprocessing(m_DM, bf, sv,pp)
# phi_CR_LIS, phi_DM_LIS = DRN.LIS_sim()
# phi_CR, phi_DMCR = DRN.TOA_sim(phi_CR_LIS, phi_DM_LIS)
# del_chi2 = DRN.del_chi2(phi_CR, phi_DMCR)
# print("del_chi2 = ",del_chi2)
print('Calling pbarlike...')
del_chi = py_pbarlike(m_DM, bf, sigma_v = sv,propagation_model=pm)
# del(keras)
print('del_chi2 = ', del_chi)
#%%


# plt.title('Cosmic Ray Antiproton Flux')
# plt.plot(E_drn, E_drn**2.7 * phi_DM_LIS[0], c = 'b', linestyle = 'dashed', label = 'DM antiproton flux at LIS')
# plt.plot(E_ams, E_ams**2.7 * phi_DMCR[0], c = 'r', linestyle = 'dashed', label = 'total antiproton flux at TOA')
# plt.plot(E_ams, E_ams**2.7 * phi_ams, c = 'g', linestyle = 'dotted', label = 'AMS antiproton flux')

# plt.legend()
# plt.xscale('log')
# plt.yscale('log')
# plt.ylabel('$\Phi \cdot E^{2.7}$ [GeV$^{1.7}$ m$^{-2}$ sr$^{-1}$ s$^{-1}$]')
# plt.xlabel('E [GeV]')
# plt.show()

# %%
