#%%

from preamble import *
from DRN_interface import *
from solar_mod import *
print("\033[32m Loaded required custom python modules - DRN_interface, solar_mod")

theta_prop = theta_set[100:102]

#%% Applying solar modulation; Adding DM and CR contributions to antiproton flux

def br_fr(input):
    bf = np.zeros((1,8))
    channels = ["q qbar", "c cbar", "b bbar", "t tbar", "W- W+", "Z0 Z0", "g g", "h h"]
    for idx,val in enumerate(channels):
        if val in input :
            bf[0,idx] = input[val] 
    return bf
    

def pbar_flux(DM_masses,sigma_v,br_fr):
    
    m_DM,bf,sigma_v, propagation_parameters = make_array(DM_masses,br_fr,sigma_v, theta_prop)
    if len(m_DM)==1:
        if len(bf)==1:
            propagation_parameters_list = theta_prop
            m_DM_list = np.repeat(m_DM,len(propagation_parameters),axis=0)
            bf_list = np.repeat(bf,len(propagation_parameters),axis=0)
        elif len(bf)>1 :
            propagation_parameters_list = np.stack([propagation_parameters]*len(bf)).reshape((len(bf)*len(propagation_parameters),np.size(propagation_parameters,1)))
            bf_list = np.repeat(bf,len(propagation_parameters),axis=0)
            m_DM_list = np.repeat(m_DM,(len(bf)*len(propagation_parameters)),axis=0)
    elif len(m_DM)>1:
        if len(bf)==1:
            propagation_parameters_list = np.stack([propagation_parameters]*len(m_DM)).reshape((len(m_DM)*len(propagation_parameters),np.size(propagation_parameters,1)))
            m_DM_list = np.repeat(m_DM,len(propagation_parameters),axis=0)
            bf_list = np.repeat(bf,(len(m_DM)*len(propagation_parameters)),axis=0)
        elif len(bf)>1:
            propagation_parameters_list = np.stack([propagation_parameters]*len(bf)*len(m_DM)).reshape((len(m_DM)*len(bf)*len(propagation_parameters),np.size(propagation_parameters,1)))
            bf_temp = np.repeat(bf,len(propagation_parameters),axis=0)
            bf_list = np.stack([bf_temp]*len(m_DM)).reshape((len(m_DM)*len(bf)*len(propagation_parameters)),np.size(bf,1))
            m_DM_list = np.repeat(m_DM,len(bf)*len(propagation_parameters),axis=0)

    print(m_DM_list[0],bf_list[0],propagation_parameters_list[0])
    phi_DM_LIS = DM_pbar_flux(m_DM_list,bf_list,sigma_v, propagation_parameters_list)
    phi_CR_LIS = CR_pbar_sim(propagation_parameters)
    phi_CR_LIS_rr = (np.stack([phi_CR_LIS]*len(m_DM)*len(bf))).reshape((len(m_DM)*len(bf)*len(propagation_parameters),np.size(phi_CR_LIS,1)))
    phi_LIS    = phi_CR_LIS_rr + phi_DM_LIS

    phi_DMCR = np.zeros((len(phi_LIS),len(E_ams)))
    phi_CR = np.zeros((len(phi_CR_LIS),len(E_ams)))
    for i in range(len(phi_LIS)):
        V_profiled,A_profiled = nuisance_estimation(phi_LIS[i], E_drn)
        phi_DMCR[i]           = solar_mod(phi_LIS[i]*A_profiled, E_drn, V_profiled )
        
    for i in range(len(phi_CR_LIS)):
        V_profiled,A_profiled = nuisance_estimation(phi_CR_LIS[i], E_drn)
        phi_CR[i]           = solar_mod(phi_CR_LIS[i]*A_profiled, E_drn, V_profiled )
    
    skip = len(propagation_parameters)
    phi_DMCR_grouped = [phi_DMCR[i*skip:(i+1)*skip] for i in range(int(len(phi_LIS)/skip))]
    
    return phi_DMCR_grouped,phi_CR

def chi2_array(phi_DMCR,phi_CR):
    chi2_DMCR = np.array([[chi2(j)for j in phi_DMCR[i]] for i in range(len(phi_DMCR))])
    chi2_CR   = np.array([chi2(j)for j in phi_CR])
    return chi2_DMCR,chi2_CR

def del_chi2(chi2_DMCR,chi2_CR):
    return -2 * np.log( 1/len(theta_prop) * np.sum(np.exp((chi2_CR - chi2_DMCR)/2),axis=1) )

def delChi2(DM_masses,sigma_v,br_fr):
    phi_DMCR,phi_CR   = pbar_flux(DM_masses,sigma_v,br_fr)
    chi2_DMCR,chi2_CR = chi2_array(phi_DMCR,phi_CR)
    delChi2Array     = del_chi2(chi2_DMCR,chi2_CR)
    return delChi2Array



print("\033[37m     Loaded pbarlike 1.0")