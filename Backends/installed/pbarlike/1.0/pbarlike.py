#%%

from preamble import *
from DRN_interface import *
# from solar_mod import *
print("\033[32m Loaded required custom python modules - DRN_interface, solar_mod")

#%% Applying solar modulation; Adding DM and CR contributions to antiproton flux

def br_fr(inputs, sigma_v=1):
    # DRBF - Dark Ray net Branching Fraction dictionary
    DRBF = {"q qbar":0, "c cbar":1, "b bbar":2, "t tbar":3, "W+ W-":4, "Z0 Z0":5, "g g":6, "h h":7}
    bf = np.zeros((1,8))
    factorized_bf = {key: inputs[key] / sigma_v
                        for key in inputs.keys()}
    # Template for the dictionary entry - Gambit annihilation channel : DRN annihilation channel
    keys_to_location = {
                    "u1 ubar_1":DRBF['q qbar'],"u2 ubar_2":DRBF['c cbar'],"u3 ubar_3":DRBF['t tbar'],
                    "ubar_1 u1":DRBF['q qbar'],"ubar_2 u2":DRBF['c cbar'],"ubar_3 u3":DRBF['t tbar'],
                    "d1 dbar_1":DRBF['q qbar'],"d2 dbar_2":DRBF['q qbar'],"d3 dbar_3":DRBF['b bbar'],
                    "dbar_1 d1":DRBF['q qbar'],"dbar_2 d2":DRBF['q qbar'],"dbar_3 d3":DRBF['b bbar'],
                    "W+ W-":4, "W- W+":4,
                    "Z0 Z0":5, "g g":6, 
                    "h h":7
                        }
    for i in keys_to_location.keys() :
        bf[0,keys_to_location[i]] += factorized_bf.get(i,0)
    return bf

def DRN_initialization(propagation_parameters,propagation_model='DIFF.BRK', prevent_extrapolation= False):
    propagation_parameters = np.array(propagation_parameters)
    print('\nPropagation model: ',DRN.propagation_model)
    print(propagation_parameters,propagation_model,prevent_extrapolation)
    DRN = DRNet(propagation_parameters,propagation_model,prevent_extrapolation)
    return DRN
    
def py_pbar_logLike_DRN(DRN, DM_mass, brfr, sigma_v = 10**(-25.5228)):
    bf = br_fr(brfr,sigma_v)
    DRN.preprocessing_DMparams(DM_mass, bf, sigma_v)
    print('Normalized branching fractions: ',DRN.bf)
    print('\n Rescaled cross-section: ', DRN.sv)
    phi_CR_LIS, phi_DM_LIS = DRN.LIS_sim()
    phi_CR, phi_DMCR = DRN.TOA_sim(phi_CR_LIS, phi_DM_LIS)
    del_chi2 = DRN.del_chi2(phi_CR, phi_DMCR)
    return del_chi2

def py_pbarlike(DM_mass, brfr, sigma_v = 10**(-25.5228), propagation_model='DIFF.BRK', propagation_parameters = None, prevent_extrapolation = False):
    DRN = DRNet(propagation_model,prevent_extrapolation)
    DRN.preprocessing(DM_mass, brfr, sigma_v, propagation_parameters)
    print('Normalized branching fractions: ',DRN.bf)
    print('\nPropagation model: ',DRN.propagation_model)
    phi_CR_LIS, phi_DM_LIS = DRN.LIS_sim()
    phi_CR, phi_DMCR = DRN.TOA_sim(phi_CR_LIS, phi_DM_LIS)
    del_chi2 = DRN.del_chi2(phi_CR, phi_DMCR)
    return del_chi2
    
# def pbar_flux(DM_masses,sigma_v,br_fr,propagation_model,propagation_parameters,marginalization):
#     DRN = DRNet(propagation_model,propagation_parameters,marginalization)
#     theta_prop = DRN.pp
#     m_DM,bf,sigma_v, propagation_parameters = DRN.make_array(DM_masses,br_fr,sigma_v, theta_prop)
#     if len(m_DM)==1:
#         if len(bf)==1:
#             propagation_parameters_list = propagation_parameters
#             m_DM_list = np.repeat(m_DM,len(propagation_parameters),axis=0)
#             bf_list = np.repeat(bf,len(propagation_parameters),axis=0)
#         elif len(bf)>1 :
#             propagation_parameters_list = np.stack([propagation_parameters]*len(bf)).reshape((len(bf)*len(propagation_parameters),np.size(propagation_parameters,1)))
#             bf_list = np.repeat(bf,len(propagation_parameters),axis=0)
#             m_DM_list = np.repeat(m_DM,(len(bf)*len(propagation_parameters)),axis=0)
#     elif len(m_DM)>1:
#         if len(bf)==1:
#             propagation_parameters_list = np.stack([propagation_parameters]*len(m_DM)).reshape((len(m_DM)*len(propagation_parameters),np.size(propagation_parameters,1)))
#             m_DM_list = np.repeat(m_DM,len(propagation_parameters),axis=0)
#             bf_list = np.repeat(bf,(len(m_DM)*len(propagation_parameters)),axis=0)
#         elif len(bf)>1:
#             propagation_parameters_list = np.stack([propagation_parameters]*len(bf)*len(m_DM)).reshape((len(m_DM)*len(bf)*len(propagation_parameters),np.size(propagation_parameters,1)))
#             bf_temp = np.repeat(bf,len(propagation_parameters),axis=0)
#             bf_list = np.stack([bf_temp]*len(m_DM)).reshape((len(m_DM)*len(bf)*len(propagation_parameters)),np.size(bf,1))
#             m_DM_list = np.repeat(m_DM,len(bf)*len(propagation_parameters),axis=0)

#     phi_DM_LIS = DRN.DM_pbar_flux(m_DM_list,bf_list,sigma_v, propagation_parameters_list)
#     phi_CR_LIS = DRN.CR_pbar_sim(propagation_parameters)
#     phi_CR_LIS_rr = (np.stack([phi_CR_LIS]*len(m_DM)*len(bf))).reshape((len(m_DM)*len(bf)*len(propagation_parameters),np.size(phi_CR_LIS,1)))
#     phi_LIS    = phi_CR_LIS_rr + phi_DM_LIS

#     phi_DMCR = np.zeros((len(phi_LIS),len(E_ams)))
#     phi_CR = np.zeros((len(phi_CR_LIS),len(E_ams)))
#     for i in range(len(phi_LIS)):
#         V_profiled,A_profiled = nuisance_estimation(phi_LIS[i], E_drn)
#         phi_DMCR[i]           = solar_mod(phi_LIS[i]*A_profiled, E_drn, V_profiled )
        
#     for i in range(len(phi_CR_LIS)):
#         V_profiled,A_profiled = nuisance_estimation(phi_CR_LIS[i], E_drn)
#         phi_CR[i]           = solar_mod(phi_CR_LIS[i]*A_profiled, E_drn, V_profiled )
    
#     skip = len(propagation_parameters)
#     phi_DMCR_grouped = [phi_DMCR[i*skip:(i+1)*skip] for i in range(int(len(phi_LIS)/skip))]
    
#     return propagation_parameters,phi_DMCR_grouped,phi_CR

# def chi2_array(phi_DMCR,phi_CR):
#     chi2_DMCR = np.array([[chi2(j)for j in phi_DMCR[i]] for i in range(len(phi_DMCR))])
#     chi2_CR   = np.array([chi2(j)for j in phi_CR])
#     return chi2_DMCR,chi2_CR

# def del_chi2(chi2_DMCR,chi2_CR,theta_prop):
#     return -2 * np.log( 1/len(theta_prop) * np.sum(np.exp((chi2_CR - chi2_DMCR)/2),axis=1) )

# def delChi2(DM_masses,sigma_v,br_fr,propagation_model,propagation_parameters,marginalization):
#     theta_prop,phi_DMCR,phi_CR   = pbar_flux(DM_masses,sigma_v,br_fr,propagation_model,propagation_parameters,marginalization)
#     chi2_DMCR,chi2_CR = chi2_array(phi_DMCR,phi_CR)
#     delChi2Array     = del_chi2(chi2_DMCR,chi2_CR,theta_prop)
#     gc.collect()
#     return delChi2Array



print("\033[37m     Loaded pbarlike 1.0")