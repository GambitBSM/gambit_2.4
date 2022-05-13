#%%
from preamble import *

#%% Helper functions
# theta_prop - (number_of_m_DM,11) array containing generally accepteed values of propagation parameters 
def usual_theta_prop(number_of_m_DM,gamma_1p = 1.80, gamma_1 = 1.79, gamma_2p = 2.405, gamma_2 = 2.357, R_0 = 7.92e3, s = 0.37, D_0 = 2.05e28, delta = 0.419, v_alfven = 8.84, v_0c = 0.09, z_h = 2.60):
    theta_prop_temp = np.array([gamma_1p, gamma_1, gamma_2p, gamma_2, R_0, s, D_0, delta, v_alfven, v_0c, z_h])
    theta_prop = np.repeat([theta_prop_temp],number_of_m_DM,axis=0)
    return theta_prop 

def make_array(DM_masses,br_fr,sigma_v, propagation_parameters):
    if type(DM_masses) == float or type(DM_masses)== np.float64 or type(DM_masses) == np.int64:
        DM_masses = np.array([DM_masses])
    if br_fr.ndim == 1:
        br_fr = br_fr[np.newaxis,:]    
    if type(sigma_v) == float or type(sigma_v)== np.float64 or type(sigma_v) == np.int64:
        sigma_v = np.array([sigma_v])
    if propagation_parameters.ndim == 1:
        propagation_parameters = propagation_parameters[np.newaxis,:]
    return DM_masses,br_fr,sigma_v, propagation_parameters
    # Outputs 1D array (DM_masses - (1,); sigma_v - (1,)) if inputs are floats, outputs 2D array if inputs are 1D array 
    # (DM_masses - (n,); br_fr - (1,8); sigma_v - (n,); propagation_parameters - (1,11))

# Check if given parameters are outside trained regions of the network
# propagation_parameters - (n,11) shaped array if flux is predicted for n DM_masses
# DM_masses - 1D array of shape (n,)
def outside_comfort_zone_DM(DM_masses):
    stop_DM = False
    #Checking DM parameters
    if np.min(DM_masses) < 5 or np.max(DM_masses) > 5e3:
        print('\n At least one of the given DM masses is outside of the provided range (5 GeV to 5 TeV). DM antiproton flux cannot be predicted.')
        stop_DM = True
    return stop_DM

def outside_comfort_zone_CR(propagation_parameters):
    stop_CR = False
    #Checking propagation parameters
    strings = ['gamma 1,p', 'gamma 1', 'gamma 2,p', 'gamma 2', 'R_0', 's_0', 'D_0', 'delta', 'v_Alfven', 'v_0,c', 'z_h']
    for i in range(11):
        if np.min(propagation_parameters[:, i]) <= mins_pp[i] or np.max(propagation_parameters[:, i]) >= maxs_pp[i]:
            #print('\n At least one of the inputs for %s is outside the trained parameter ranges. No output will be given. '%strings[i])
            print(np.min(propagation_parameters[:, i]),np.max(propagation_parameters[:, i]))
            stop_CR = True
            break
    return stop_CR

#br_fr - 2D array of shape (n,8), since 8 annihilation channels are included in the network. Note br_fr \in [1e-5,1)
def input_preprocessing_DM(DM_masses,br_fr,propagation_parameters):
    # Min-max standardization of DM masses
    m_DM = ((np.log10(DM_masses)+3) - np.log10(5e3)) / (np.log10(5e6)-np.log10(5e3))
    # Replacing zeros by 1e-5
    bf_temp = np.where(br_fr<1e-5,1e-5,br_fr)
    # Normalizing branching fractions
    bf_temp = bf_temp / np.sum(bf_temp,axis=1)[:,None]
    # Processing braching fractions 
    bf = (np.log10(bf_temp) - (DM_trafos[1,0])) / ((DM_trafos[1,1])- (DM_trafos[1,0]))
    # Processing propagation parameters
    theta_prop = ((propagation_parameters - temp1[:11])/temp2[:11])
    return m_DM,bf,theta_prop
    # Outputs processed 1D (m_DM) and 2D (bf,theta_prop)

# Processing inputs, obtaining prediction from DMNet, and postprocessing prediction:
def DRN_output(DM_masses,br_fr,propagation_parameters): 
    # Input preprocessing  
    m_DM,bf,theta_prop = input_preprocessing_DM(DM_masses,br_fr,propagation_parameters)
    # x - (n,40) array of x values at which network predicts output ; x = E/m_DM
    x_temp = 10**(np.linspace(-3.7, 0, 40))
    x = np.repeat([x_temp],len(m_DM),axis=0)
    # E_dmnet - (n,40) array of kinetic energy(KE) per nucleon values at which network predicts output
    E_dmnet = x*DM_masses[:,None]
    # y_DM_x - (n,40) array of y values predicted by the DMNet for different x values; y(x) = log10(m^3 x phi(E))
    
    DM_model = keras.models.load_model(script_directory+'/dependencies/DM_model_x.h5')
    y_DM_x = DM_model([m_DM,bf,theta_prop])
    
    keras.backend.clear_session()
    del DM_model    
    gc.collect()

    # phi_dmnet - (n,40) array of flux values predicted by the DMNet for different KE per nucleon values
    phi_dmnet = 10**(y_DM_x) / (DM_masses[:,None]**3 * x)
    # phi_DM_LIS - (n,28) array of flux values interpolated to obtain fluxes at the same KE per nucleon values as in E_drn so that it can
    # be added to phi_CR_LIS. Only after solar modulation, the flux is to be interpolated to E_ams values.
    phi_DM_LIS = np.zeros((len(m_DM),len(E_drn)))
    for i in range(len(DM_masses)):
        E_drn_allowed = []
        indices = []
        # Flux predicted by DMNet is only reliable in the allowed x range, i.e. for only those KE per nucleon in E_drn 
        # which for a given DM mass fall within the allowed x*m values. Thus we make a list of these allowed E_drn values, interpolate
        # the flux only at these values and set all other flux values at other E_drn to zero.
        for j in range(len(E_drn)):
            if E_drn[j]/DM_masses[i] >= 10**(-3.7) and E_drn[j]/DM_masses[i] <= 1:
                E_drn_allowed.append(E_drn[j])
                indices.append(j)
        phi_DM_LIS[i,indices]  = np.exp(np.interp(np.log(E_drn_allowed), np.log(E_dmnet[i]), np.log(phi_dmnet[i])) )
        #f = interp1d(np.log(E_dmnet[i]), np.log(phi_dmnet[i]))
        #phi_DM_LIS[i,indices]  = np.exp(f(np.log(E_drn_allowed)) )
    return phi_DM_LIS
    # Outputs 2D phi_DM_LIS - (n,58) array of flux values interpolated to obtain fluxes at the same KE per nucleon values as in E_drn

# sigma_v - (n,) array or scalar float containing cross section values 
def DM_pbar_flux(DM_masses,br_fr,sigma_v, propagation_parameters):
    DM_masses,br_fr,sigma_v, propagation_parameters = make_array(DM_masses,br_fr,sigma_v, propagation_parameters)
    stop_DM = outside_comfort_zone_DM(DM_masses)
    stop_CR = outside_comfort_zone_CR(propagation_parameters)
    if stop_DM  or stop_CR :
        print('The DM antiproton flux cannot be predicted by DRN due to atleast one parameter outside the trained regions.')
    else:
        phi_DM_LIS = DRN_output(DM_masses,br_fr,propagation_parameters)
        if type(sigma_v) == float or type(sigma_v)== np.float64 or type(sigma_v) == np.int64:
            sigma_v = np.array([sigma_v])
        return sigma_v[:,None]/10**(-25.5228)*phi_DM_LIS
        # Outputs 2D phi_DM_LIS - (n,28) array of flux values interpolated to obtain fluxes at the same KE per nucleon values as in E_drn
        
def CR_pbar_sim(propagation_parameters):
    if propagation_parameters.ndim == 1 :
        propagation_parameters = propagation_parameters[np.newaxis,:]
    stop = outside_comfort_zone_CR(propagation_parameters)
    if stop == True:
        print('The secondary antiproton flux cannot be predicted by DRN due to atleast one propagation parameter outside the trained regions')
    else:        
        # preprocessing propagation parameters
        theta_prop = (( propagation_parameters - (S_trafos[0])[:11])/(S_trafos[1])[:11])
        # y_CR - (28,) array of y values predicted by the sNet at different KE per nucleon values in E_drn ; y(E) = log10(E^2.7 phi(E))

        S_model = keras.models.load_model(script_directory+'/dependencies/S_model.h5')
        y_CR = S_model(theta_prop)

        keras.backend.clear_session()
        del S_model
        gc.collect()

        # phi_CR_LIS - (28,) array of flux values predicted by the sNet at different KE per nucleon values in E_drn
        phi_CR_LIS = 10**(y_CR)/E_drn**2.7
        return phi_CR_LIS
        # Outputs phi_CR_LIS - (28,) array of flux values predicted by the sNet at different KE per nucleon values in E_drn
