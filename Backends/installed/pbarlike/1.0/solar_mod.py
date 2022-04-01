#%%
from preamble import *
from DRN_interface import *

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


@njit
def chi2(phi_pred):
    return np.sum((phi_ams - phi_pred)**2 / error_ams**2)
    
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