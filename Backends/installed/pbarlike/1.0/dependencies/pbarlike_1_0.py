# %%
import numpy as np
from numba import njit
import tensorflow as tf
from iminuit import Minuit
import matplotlib.pyplot as plt
m_p = 938.27208816 * 1e-3 #GeV

#%%
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
# ## Converting R to kinetic energy per nucleon: 
# $$ E_k / A = \sqrt{(RZ/A)^2 + m^2} $$
# 
# ## Converting flux in R to flux in E :
# $$ \frac{d \phi}{d R} \frac{d R}{d (E_k/A)} = \frac{d \phi}{d R} \frac{1}{R} (\frac{A}{Z})^2 \sqrt{( \frac{RA}{Z})^2 + m^2} $$

# %%
# Loading 7 year ams02_data which has columns as above :
ams_data = np.genfromtxt('dependencies/pbar_ams02_7y.txt')

# R - array with rigidity values in GV; outputs array of E_k in GeV
def R_to_Ekin(R,z = -1, A = 1, m = m_p ):
    Z = np.abs(z)
    return np.sqrt(R**2 * (Z/A)**2 + m**2) - m

# flux_in_R - array of flux values measured at different rigidities in m-2 sr-1 s-1 GV-1; outputs array of flux values measured at
# different energies in m-2 sr-1 s-1 GeV-1
def flux_in_Ekin(flux_in_R,R,z=-1,A=1,m=m_p):
    Z = np.abs(z)
    return flux_in_R /R * (A/Z)**2 * np.sqrt((R*Z/A)**2 + m**2)

R_ams = (ams_data[:,0]+ams_data[:,1])/2 
phi_in_R = ams_data[:,10] * ams_data[:,13]
error_R_ams = np.sqrt(ams_data[:,11]**2+ams_data[:,12]**2)*ams_data[:,13]

# E_ams -(58,) array of KE per nucleon values at which AMS02 flux measurements are recorded for pbar
E_ams = np.array(R_to_Ekin(R_ams))
# phi_ams - (58,) array of flux values measured at E_ams
phi_ams = np.array(flux_in_Ekin(phi_in_R,R_ams))
# Error propagation gives the error of flux in E_k as follows:
# error_ams - (58,) array of error values of flux values measured at E_ams
error_ams = np.array(flux_in_Ekin(error_R_ams,R_ams))


# E_drn - (28,) array of KE per nucleon values at which sNet predicts flux values (same values at which training data are given to the sNet)
E_drn = np.array(np.load('dependencies/E.npy'))

# Loading DMNet 
DM_model = tf.keras.models.load_model('dependencies/DM_model_x.h5')
# Loading transformations for propagation parameters for DMNet
DM_trafos =np.array(np.load('dependencies/DM_trafos_x.npy', allow_pickle = True))
temp1 = np.array(DM_trafos[0,0])
temp2 = np.array(DM_trafos[0,1])

# Loading sNet
S_model = tf.keras.models.load_model('dependencies/S_model.h5')
# Loading transformations for propagation parameters for sNet
S_trafos = np.array(np.load('dependencies/S_trafos.npy', allow_pickle = True))

# Loading priors
mins_pp = np.array([1.2, 1.2, 2.3, 2.3, 1000, 0.1, 5e27, 0.2, 0, 0, 2])
maxs_pp = np.array([2, 2, 2.6, 2.6, 20000, 0.9, 1e29, 0.6, 30, 60, 7])

#%%
# # Chi squared function ($\chi^2$) : 
# Calculates the chi sqaured function, sqaured differences between the flux observed by AMS at energies E_ams and flux predicted at these same energies.
# $$\chi^2 (x_{DM},\theta_{prop}) = \sum_i^{len(E_{ams})} \frac{(\phi_{ams}(E_i) - (\phi_{DM}(E_i)+\phi_{CR}(E_i)))^2}{\sigma_{ams}^2}$$
# $$\chi^2 (\theta_{prop}) = \sum_i^{len(E_{ams})} \frac{(\phi_{ams}(E_i) - \phi_{CR}(E_i))^2}{\sigma_{ams}^2}$$
# 
# The measurement error from AMS is given by,
# 
# $$ \sigma_{ams} = \sqrt{\sigma_{sys}^2 + \sigma_{stat}^2 }$$
#  
#To calculate chi^2 (x_{DM},\theta_{prop}), use phi_pred - array of (phi_{CR}+phi_{DM}) values at energies E_ams 
#To calculate chi^2 (\theta_{prop}), use phi_pred - array of phi_{CR} values at energies E_ams 

@njit
def chi2(phi_pred):
    return np.sum((phi_ams - phi_pred)**2 / error_ams**2)

# %% [markdown]
# # Predicting Antiproton flux in the local interstellar region ($\phi_{LIS}$) :
# The antiproton flux receives contribution from DM annihilations and secondary production by interaction of cosmic rays with interstellar medium. $$\phi_{LIS} = \phi_{DM,LIS} + \phi_{CR,LIS} $$
# ### DM contribution ($\phi_{DM,LIS}$) :
# The antiproton flux from DM annihilations in the local interstellar region is predicted by solving propagation equation, simulated using DRN. DRN takes branching fractions ($bf \in [1e-5,1); \sum bf = 1$), DM mass (MeV) and propagation parameters ($\theta_{prop}$) as inputs. These inputs are preprocessed as follows:
# $$ m_{DM} \rightarrow \frac{(log_{10}(m_{DM}) - log_{10}(m_{DM,min}/MeV))}{(log_{10}(m_{DM,max}/MeV) - log_{10}(m_{DM,min}/MeV))}  $$
# where $m_{DM,min} = 5 GeV; m_{DM,max} = 5 TeV $.
# 
# The network was trained on interesting parameter ranges and hence does not give reliable output for parameters outside this range. Thus, the propagation parameters should be checked if they are inside the allowed regions.
# 
# ### Secondary CR contribution ($\phi_{CR,LIS}$) :
# The antiproton flux from CR interactions in the ISM in the local interstellar region is calculated using sNET which takes propagation parameters as input.
# 

# %%
# Making arrays of scalar inputs, this makes it easier to use the following functions for both scalars and arrays

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

# theta_prop - (number_of_m_DM,11) array containing generally accepteed values of propagation parameters 
def usual_theta_prop(number_of_m_DM,gamma_1p = 1.80, gamma_1 = 1.79, gamma_2p = 2.405, gamma_2 = 2.357, R_0 = 7.92e3, s = 0.37, D_0 = 2.05e28, delta = 0.419, v_alfven = 8.84, v_0c = 0.09, z_h = 2.60):
    theta_prop_temp = np.array([gamma_1p, gamma_1, gamma_2p, gamma_2, R_0, s, D_0, delta, v_alfven, v_0c, z_h])
    theta_prop = np.repeat([theta_prop_temp],number_of_m_DM,axis=0)
    return theta_prop 

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

# br_fr - 2D array of shape (n,8), since 8 annihilation channels are included in the network. Note br_fr \in [1e-5,1)

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
def DM_pbar_flux(DM_masses,br_fr,propagation_parameters): 
    # Input preprocessing  
    m_DM,bf,theta_prop = input_preprocessing_DM(DM_masses,br_fr,propagation_parameters)
    # x - (n,40) array of x values at which network predicts output ; x = E/m_DM
    x_temp = 10**(np.linspace(-3.7, 0, 40))
    x = np.repeat([x_temp],len(m_DM),axis=0)
    # E_dmnet - (n,40) array of kinetic energy(KE) per nucleon values at which network predicts output
    E_dmnet = x*DM_masses[:,None]
    # y_DM_x - (n,40) array of y values predicted by the DMNet for different x values; y(x) = log10(m^3 x phi(E))
    y_DM_x = DM_model.predict([m_DM,bf,theta_prop])
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
def DM_pbar_sim(DM_masses,br_fr,sigma_v, propagation_parameters):
    DM_masses,br_fr,sigma_v, propagation_parameters = make_array(DM_masses,br_fr,sigma_v, propagation_parameters)
    stop_DM = outside_comfort_zone_DM(DM_masses)
    stop_CR = outside_comfort_zone_CR(propagation_parameters)
    if stop_DM  or stop_CR :
        print('The DM antiproton flux cannot be predicted by DRN due to atleast one parameter outside the trained regions.')
    else:
        phi_DM_LIS = DM_pbar_flux(DM_masses,br_fr,propagation_parameters)
        return sigma_v,phi_DM_LIS
        # Outputs 2D phi_DM_LIS - (n,28) array of flux values interpolated to obtain fluxes at the same KE per nucleon values as in E_drn
        
def DM_final(sigma_v,phi_DRN):
    if type(sigma_v) == float or type(sigma_v)== np.float64 or type(sigma_v) == np.int64:
        sigma_v = np.array([sigma_v])
    return sigma_v[:,None]/10**(-25.5228)*phi_DRN


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
        y_CR = S_model.predict(theta_prop)
        # phi_CR_LIS - (28,) array of flux values predicted by the sNet at different KE per nucleon values in E_drn
        phi_CR_LIS = 10**(y_CR)/E_drn**2.7
        return phi_CR_LIS
        # Outputs phi_CR_LIS - (28,) array of flux values predicted by the sNet at different KE per nucleon values in E_drn


# %% [markdown]
# # Predicting Antiproton flux measured by AMS ($\phi_{mod}$) :
# The antiprotons that are measured by the AMS have entered the heliosphere and are hence subject to solar modulation. We use the force field approximation to obtain the solar modulated flux $\phi_{mod}$,
# $$ \phi_{DM} = \phi_{DM,LIS} \frac{E_{ams}^2 + 2E_{ams}m}{E_{LIS}^2 + 2E_{LIS}m}  $$
# $$ \phi_{CR} = \phi_{CR,LIS} \frac{E_{ams}^2 + 2E_{ams}m}{E_{LIS}^2 + 2E_{LIS}m}  $$
# $$ E_{LIS} = E_{ams} + \frac{|Z|}{A} V(E_{LIS}) $$
# where $V(E_{LIS})$ is the solar modulation potential. Since the potential is not known, it is estimated by minimizing the $\chi^2$ function for a particular set of DM and propagation parameters ($ branching ratios,m_{DM}, \theta_{prop} $). 
# 
# ## Profiling the $\chi^2$ function to estimate the solar modulation potential : 
# The solar modulation potential is treated as a nuisance parameter and is estimated while calculating the profile likelihood, i.e. its value is set equal to the one that maximizes the likelihood ($\hat{V}$), for each value of the relevant parameters. The profile likelihood or the corresponding profile $\chi^2$ is given by,
# $$profile \ \chi^2 = \sum_{i=1}^N \frac{(\phi_{ams}-\phi(\theta_{prop},\hat{V}))^2}{\sigma^2}$$

# E_ams - (58,) array of KE per nucleon values at which AMS02 records measurement of flux values for pbar
# phi_LIS - (58,) array of flux values predicted by DMNet or sNet at E
# E_LIS - (28,) array of KE per nucleon values at which NN predicts fluxes (E_drn)
# V - solar modulation potential in giga volts 

def solar_mod(phi_LIS, E_LIS, V, Z=-1., A=1., m=m_p ):
    # E_LIS_ams - (58,) array of KE per nucleon values at LIS which after solar modulation reduce to E_ams
    E_LIS_ams = E_ams + np.abs(Z)/A * V
    # phi_LIS_interp - (n,58) array of flux values interpolated to the above E values.
    phi_LIS_interp = np.ones((len(phi_LIS),len(E_ams)))
    for i in range(len(phi_LIS)):
        phi_LIS_interp[i] = (np.interp(E_LIS_ams,E_LIS,phi_LIS[i]))
    # phi_earth - (n,58) array of flux values simulated as that which is measured by AMS 02, i.e., flux after solar modulation
    phi_earth = phi_LIS_interp * (E_ams**2 + 2*E_ams*m)/(E_LIS_ams**2 + 2*E_LIS_ams*m)
    return phi_earth
    # Outputs (n,58) array containing flux values that need to be compared with AMS 02 measurements


# Profiling chi2 to find the solar modulation potential that minimizes the chi2:
def nuisance_estimation(phi_LIS, E_LIS):
    # Defining the function to be minimized to profile the nuisance parameters
    def lsq(nuisance_parameters):
        # V - solar modulation potential
        V = nuisance_parameters[0]
        # A - normalization constant
        A = nuisance_parameters[1]
        # phi_pred - (1,58) array containing values of normalized, solar modulated flux values 
        phi_pred = solar_mod(phi_LIS*A, E_LIS, V )
        # chi2_temp - a scalar value of weighted sum of squared difference between the normalized, solar modulated flux and flux measured by AMS02
        chi2_temp = chi2(phi_pred)
        return chi2_temp
    
    # Initiating whatever to minimize the chi squared function
    profiling = Minuit. from_array_func(fcn   = lsq ,#grad = jax.grad(lsq), # lsq - function to be minimized
                                        start = np.array([0.6, 1]), # starting values for the parameters to be estimated
                                        error = np.array([0.001, 0.001]), # step sizes for gradient descent for each of the parameters
                                        limit = np.array([[0.2, 0.9], [0.1, 5]]), # allowed interval for the parameters
                                        #name  = np.array(['phi_AMS-02_pbar', 'norm_AMS-02_pbar']), 
                                        errordef=1) # errordef = 1 for Least squares function; errordef = 0.5 for maximum likelihood function
    
    # migrad - algorithm for gradient descent (?)
    profiling.migrad()
    # V_profiled,A-profiled - values of estimated parameters
    V_profiled,A_profiled = profiling.np_values()
    return V_profiled,A_profiled
    # Outputs V_profiled,A-profiled - values of estimated parameters

#%% Applying solar modulation; Adding DM and CR contributions to antiproton flux

def pbar_flux(DM_masses,br_fr,sigma_v):
    propagation_parameters = usual_theta_prop(1)
    sigma_v,phi_DRN = DM_pbar_sim(DM_masses,br_fr,sigma_v, propagation_parameters)
    phi_DM_LIS      = DM_final(sigma_v,phi_DRN)
    phi_CR_LIS      = CR_pbar_sim(propagation_parameters)
    phi_LIS         = phi_CR_LIS + phi_DM_LIS

    V_profiled,A_profiled = nuisance_estimation(phi_LIS, E_drn)
    phi_DMCR              = solar_mod(phi_LIS*A_profiled, E_drn, V_profiled )

    return phi_DMCR[0]
    

#%% Check

m_DM = 100.
bf = np.array([0,0,1,0,0,0,0,0])
sv = 10**(-25.5228)
pp = np.array([1.8, 1.79, 2.405, 2.357, 7.92e+03, 0.37, 2.05e+28, 0.419, 8.84, 0.09, 2.60])

pbar_flux_pred = pbar_flux(m_DM,bf,sv)
loglike = chi2(pbar_flux_pred)




plt.title('Cosmic Ray Antiproton Flux')
plt.plot(E_ams, E_ams**2.7 * pbar_flux_pred, c = 'b', linestyle = 'dashed', label = 'total predicted antiproton flux')
plt.plot(E_ams, E_ams**2.7 * phi_ams, c = 'g', linestyle = 'dotted', label = 'AMS antiproton flux')

plt.legend()
plt.xscale('log')
plt.yscale('log')
plt.ylabel('$\Phi \cdot E^{2.7}$ [GeV$^{1.7}$ m$^{-2}$ sr$^{-1}$ s$^{-1}$]')
plt.xlabel('E [GeV]')
plt.show()
