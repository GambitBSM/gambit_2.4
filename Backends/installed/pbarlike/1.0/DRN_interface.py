#%%
from preamble import *

#%% Helper functions

class DRNet:
    def __init__(self,propagation_model,prevent_extrapolation):
        self.pe = prevent_extrapolation
        model_options = ['run1', 'DIFF.BRK', 'INJ.BRK+vA']
        if propagation_model in model_options:
            self.propagation_model = propagation_model
            self.dep_path = script_directory + '/dependencies/' + self.propagation_model + '/'
        else:
            print()
            print('The propagation model "%s" is not provided in this tool. It will be set to default (DIFF.BRK).'%model_name)
            self.propagation_model = 'DIFF.BRK'
        self.load_deps()
        self.load_pp_data()
        print('\n The simulation tool has been initiated. \n')

    def load_deps(self):
        # DM antiprotons    
        self.DM_trafos = np.load(self.dep_path + 'DM_trafos_x.npy', allow_pickle = True)
        # Secondary antiprotons        
        self.S_trafos = np.load(self.dep_path + 'S_trafos.npy', allow_pickle = True)
    
    def load_pp_data(self):
        # Setting length of propagation parameters according to diffusion model
        N_pp = {'run1': 11, 'DIFF.BRK': 10, 'INJ.BRK+vA': 12}
        self.N_pp = N_pp[self.propagation_model]
        # Defining names of parameters
        self.strings = {'run1' : ['gamma 1,p', 'gamma 1', 'gamma 2,p', 'gamma 2', 'R_0', 's_0', 'D_0', 'delta', 'v_Alfven', 'v_0,c', 'z_h'],
                   'DIFF.BRK' : ['gamma 2,p', 'gamma 2', 'D0xx', 'delta_l', 'delta', 'delta_h - delta', 'R_D0', 's_D', 'R_D1', 'v_0'],
                   'INJ.BRK+vA': ['gamma 1,p', 'gamma 1', 'R_0,inj', 's', 'gamma 2,p', 'gamma 2', 'D_0', 'delta', 'delta_h - delta', 'R_1D', 'v_0', 'v_Alfven']} # TO DO: add
        # Loading priors
        mins_pp = {'run1' : np.array([1.63, 1.6, 2.38, 2.34, 5000, 0.32, 1.15e28, 0.375, 0, 0, 2]),
                   'DIFF.BRK' : np.array([2.249, 2.194, 3.411e+28, -9.66635e-01, 4.794e-01, -2.000e-01, 3.044e+03, 3.127e-01, 1.217e+0, 0]),
                   'INJ.BRK+vA': []}# TO DO: add
        maxs_pp = {'run1' : np.array([1.9, 1.88, 2.45, 2.4, 9500, 0.5, 5.2e28, 0.446, 11, 12.8, 7]),
                   'DIFF.BRK' : np.array([2.37e+00, 2.314e+00, 4.454e+28, -3.677e-01, 6.048e-01, -8.330e-02, 4.928e+03, 5.142e-01, 3.154e+05, 1.447e+01]),
                   'INJ.BRK+vA': []}# TO DO: add
        self.mins_pp = mins_pp[self.propagation_model]
        self.maxs_pp = maxs_pp[self.propagation_model]
        # Loading multinest sample for the corresponding diffusion model
        self.mns = (np.genfromtxt(self.dep_path + 'multinest_sample.dat'))
        self.mnpp = self.mns[100:102,:self.N_pp]

    # propagation_parameters - (n,N_pp) shaped array if flux is predicted for n DM_masses
    # DM_masses - 1D array of shape (n,)
    def preprocessing(self, DM_mass, br_fr, sigma_v, propagation_parameters):
        # Making the propagation parameters a numpy array. Required transformations will be done within simulations (DM_sim, CR_sim)
        if propagation_parameters is not None :
            # Converting propagation parameters to 2D array
            if propagation_parameters.ndim == 1:
                propagation_parameters = propagation_parameters[np.newaxis,:]
            self.pp = propagation_parameters
            self.marginalization = False
        else :
            self.pp = self.mnpp   
            self.marginalization = True     

        # Checking if the given parameter is inside the region in which the network is trained
        if (self.pp).shape[-1] != self.N_pp:
            print('The number of propagation parameters is not consistent with the propagation model. The default multinest sample will be used for marginalization.')
            self.pp = self.mnpp    
            self.marginalization = True     


        # Preparing DM masses
        # Converting float DM mass to 1D array
        if type(DM_mass) == float or type(DM_mass)== np.float64 or type(DM_mass) == np.int64:
            DM_mass = np.array([DM_mass])
        self.DM_mass = DM_mass
        # Min-max standardization of DM masses
        m_DM = ((np.log10(DM_mass)+3) - np.log10(5e3)) / (np.log10(5e6)-np.log10(5e3))
        # Repeating mass for the given number of propagation parameter points
        m_DM = np.repeat(m_DM,len(self.pp),axis=0)
        self.m_DM = m_DM

        # Preparing branching fractions
        # Converting 1D bf array to 2D array
        if br_fr.ndim == 1:
            br_fr = br_fr[np.newaxis,:] 
        # Replacing zeros by 1e-5
        bf_temp = np.where(br_fr<1e-5,1e-5,br_fr)
        # Normalizing branching fractions
        bf_temp = bf_temp / np.sum(bf_temp,axis=-1)[:,None]
        # Processing braching fractions 
        bf = (np.log10(bf_temp) - np.array(self.DM_trafos[1,0])) / (np.array(self.DM_trafos[1,1])- np.array(self.DM_trafos[1,0])) 
        bf_r = np.repeat(bf,len(self.pp),axis=0)
        self.bf = bf_r
        
        # Preventing extrapolation
        self.stop_sim = False
        if self.pe:
            for i in range(self.N_pp):
                if np.min(self.pp[:, i]) <= self.mins_pp[i] or np.max(self.pp[:, i]) >= self.maxs_pp[i]:
                    print("At least one of the inputs for %s is outside the trained parameter ranges. No output will be given. The default multinest sample will be used for marginalization. " % (self.strings[self.propagation_model])[i])
                    print(np.min(self.pp[:, i]),np.max(self.pp[:, i]))
                    self.pp = self.mnpp
                    break   
            # Checking if the given parameter is inside the region in which the network is trained
            stop_DM = False
            #Checking DM parameters
            if np.min(self.DM_mass) < 5 or np.max(self.DM_mass) > 5e3:
                print('\n At least one of the given DM masses is outside of the provided range (5 GeV to 5 TeV). DM antiproton flux cannot be predicted.')
                stop_DM = True
            # Checking if the given parameter is inside the region in which the network is trained
            if np.min(bf) < 1e-5 or np.max(bf) > 1 :
                print(bf)
                print('The given branching fractions were not in the range of trained parameters or not normalized to one. Values below 1e-5 were mapped to 1e-5 and the remaining fractions normalized accordingly.')
                stop_DM = True 
            if stop_DM:
                self.stop_sim = True
            

        # Preparing thermally averaged annihilation cross-section (sigma_v)
        # Converting float sigma_v to 1D array
        if type(sigma_v) == float or type(sigma_v)== np.float64 or type(sigma_v) == np.int64:
            sigma_v = np.array([sigma_v])
        self.sv = sigma_v
        
    # Processing inputs, obtaining prediction from DMNet, and postprocessing prediction:
    def DM_sim(self): 
        # Transforming propagation parameters
        pp = ((self.pp - np.array(self.DM_trafos[0,0]))/np.array(self.DM_trafos[0,1]))
        # x - (n,40) array of x values at which network predicts output ; x = E/m_DM
        if self.propagation_model == 'run1':
            min_x = 0
        else: 
            min_x = -0.1 # Necessary for model without reacceleration (DM antiproton spectra diverge for E -> m_DM, x -> 0)
        x_temp = 10**(np.linspace(-3.7, min_x, 40))
        x = np.repeat([x_temp],len(self.m_DM),axis=0)
        # E_dmnet - (n,40) array of kinetic energy(KE) per nucleon values at which network predicts output
        E_dmnet = x*self.DM_mass[:,None]
        # y_DM_x - (n,40) array of y values predicted by the DMNet for different x values; y(x) = log10(m^3 x phi(E))        
        # DM_model = keras.models.load_model(self.DM_model)
        DM_model = keras.models.load_model(self.dep_path + 'DM_model_x.h5')
        y_DM_x = DM_model([self.m_DM,self.bf,pp])
        # Releasing memory
        keras.backend.clear_session()
        del DM_model    
        gc.collect()
        # phi_dmnet - (n,40) array of flux values predicted by the DMNet for different KE per nucleon values
        phi_dmnet = 10**(y_DM_x) / (self.DM_mass[:,None]**3 * x)
        # phi_DM_LIS - (n,28) array of flux values interpolated to obtain fluxes at the same KE per nucleon values as in E_drn so that it can
        # be added to phi_CR_LIS. Only after solar modulation, the flux is to be interpolated to E_ams values.
        phi_DM_LIS = np.zeros((len(self.m_DM),len(E_drn)))
        E_drn_allowed = []
        indices = []
        # Flux predicted by DMNet is only reliable in the allowed x range, i.e. for only those KE per nucleon in E_drn 
        # which for a given DM mass fall within the allowed x*m values. Thus we make a list of these allowed E_drn values, interpolate
        # the flux only at these values and set all other flux values at other E_drn to zero.
        for j in range(len(E_drn)):
            if E_drn[j]/self.DM_mass >= 10**(-3.7) and E_drn[j]/self.DM_mass <= 1:
                E_drn_allowed.append(E_drn[j])
                indices.append(j)
        for i in range(len(self.m_DM)):
            phi_DM_LIS[i,indices]  = np.exp(np.interp(np.log(E_drn_allowed), np.log(E_dmnet[i]), np.log(phi_dmnet[i])) )
        return phi_DM_LIS
        # Outputs 2D phi_DM_LIS - (n,58) array of flux values interpolated to obtain fluxes at the same KE per nucleon values as in E_drn

    def CR_sim(self):  
        # Preprocessing propagation parameters
        pp = (( self.pp - (self.S_trafos[0])[:11])/(self.S_trafos[1])[:11])
        # y_CR - (28,) array of y values predicted by the sNet at different KE per nucleon values in E_drn ; y(E) = log10(E^2.7 phi(E))
        # S_model = keras.models.load_model(self.S_model)
        S_model =keras.models.load_model(self.dep_path + 'S_model.h5')
        y_CR = S_model(pp)
        # Releasing memory
        keras.backend.clear_session()
        del S_model
        gc.collect()
        # phi_CR_LIS - (28,) array of flux values predicted by the sNet at different KE per nucleon values in E_drn
        phi_CR_LIS = 10**(y_CR)/E_drn**2.7
        return phi_CR_LIS
        # Outputs phi_CR_LIS - (28,) array of flux values predicted by the sNet at different KE per nucleon values in E_drn

    def LIS_sim(self):
        phi_CR_LIS = self.CR_sim()
        if self.stop_sim :
            print('The DM antiproton flux cannot be predicted by DRN due to atleast one parameter outside the region in which the network is trained.')
        else:
            DRN_output = self.DM_sim()
            phi_DM_LIS = self.sv[:,None]/10**(-25.5228)*DRN_output
            # Outputs 2D phi_DM_LIS - (n,28) array of flux values interpolated to obtain fluxes at the same KE per nucleon values as in E_drn
        return phi_CR_LIS, phi_DM_LIS

    # Calculates chi2 using ams data, ams errors and predicted flux
    def chi2(self,phi_pred):
        return np.sum((phi_ams - phi_pred)**2 / error_ams**2,axis = -1)
    
    # Applying solar modulation to flux predicted at the local interstellar medium
    def solar_mod(self,phi_LIS, V, Z=-1., A=1., m=m_p ):
        # E_LIS_ams - (58,) array of KE per nucleon values at LIS which after solar modulation reduce to E_ams
        E_LIS_ams = E_ams + np.abs(Z)/A * V
        # phi_LIS_interp - (n,58) array of flux values interpolated to the above E values.
        phi_LIS_interp = (np.interp(E_LIS_ams,E_drn,phi_LIS))
        # phi_earth - (n,58) array of flux values simulated as that which is measured by AMS 02, i.e., flux after solar modulation
        phi_earth = phi_LIS_interp * (E_ams**2 + 2*E_ams*m)/(E_LIS_ams**2 + 2*E_LIS_ams*m)
        return phi_earth
        # Outputs (n,58) array containing flux values that need to be compared with AMS 02 measurements

    def nuisance_estimation(self,phi_LIS):
        # Defining the function to be minimized to profile the nuisance parameters
        def lsq(nuisance_parameters):
            # V - solar modulation potential
            V = nuisance_parameters[0]
            # A - normalization constant
            A = nuisance_parameters[1]
            # phi_pred - (1,58) array containing values of normalized, solar modulated flux values 
            phi_pred = self.solar_mod(phi_LIS*A, V )
            # chi2_temp - a scalar value of weighted sum of squared difference between the normalized, solar modulated flux and flux measured by AMS02
            chi2_temp = self.chi2(phi_pred)
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
        return self.solar_mod(phi_LIS*A_profiled, V_profiled)
        # Outputs V_profiled,A-profiled - values of estimated parameters

    def nuisance_estimation_brk(self,phi_LIS):
        # Defining the function to be minimized to profile the nuisance parameters
        def lsq(V):
            # V - solar modulation potential
            # phi_pred - (1,58) array containing values of normalized, solar modulated flux values 
            phi_pred = self.solar_mod(phi_LIS, V )
            # chi2_temp - a scalar value of weighted sum of squared difference between the normalized, solar modulated flux and flux measured by AMS02
            chi2_temp = self.chi2(phi_pred)
            return chi2_temp
        
        # Initiating whatever to minimize the chi squared function
        profiling = Minuit. from_array_func(fcn   = lsq ,#grad = jax.grad(lsq), # lsq - function to be minimized
                                            start = np.array([0.6]), # starting values for the parameters to be estimated
                                            error = np.array([0.001]), # step sizes for gradient descent for each of the parameters
                                            limit = np.array([[0.2, 0.9]]), # allowed interval for the parameters
                                            #name  = np.array(['phi_AMS-02_pbar', 'norm_AMS-02_pbar']), 
                                            errordef=1) # errordef = 1 for Least squares function; errordef = 0.5 for maximum likelihood function
        
        # migrad - algorithm for gradient descent (?)
        profiling.migrad()
        # V_profiled,A-profiled - values of estimated parameters
        V_profiled = profiling.np_values()
        print(V_profiled)
        return self.solar_mod(phi_LIS, V_profiled)
        # Outputs V_profiled,A-profiled - values of estimated parameters
    
    def solar_mod_run1(self,phi_LIS):
        return np.array([self.nuisance_estimation(phi_LIS[i]) for i in range(len(phi_LIS))])

    def solar_mod_brk(self, phi_LIS):
        phi = {'DIFF.BRK': 12, 'INJ.BRK+vA': 14}
        delta_phi_bar = {'DIFF.BRK': 13, 'INJ.BRK+vA': 15}
        V = self.mns[100:102,phi[self.propagation_model]] + self.mns[100:102,delta_phi_bar[self.propagation_model]]
        if self.marginalization:
            return np.array([self.solar_mod(phi_LIS[i],V[i]) for i in range(len(phi_LIS))])
        else:
            return np.array([self.nuisance_estimation_brk(phi_LIS[i]) for i in range(len(phi_LIS))])
            # return np.array([self.solar_mod(phi_LIS[i],0.6028438883928189) for i in range(len(phi_LIS))])

    def TOA_sim(self, phi_CR_LIS, phi_DM_LIS):
        phi_LIS = phi_CR_LIS + phi_DM_LIS
        solar_modulation = {'run1':self.solar_mod_run1, 'DIFF.BRK':self.solar_mod_brk, 'INJ.BRK+vA':self.solar_mod_brk}
        phi_CR   = solar_modulation[self.propagation_model](phi_CR_LIS) 
        phi_DMCR = solar_modulation[self.propagation_model](phi_LIS) 
        return phi_CR, phi_DMCR
    
    def del_chi2(self,phi_CR, phi_DMCR):
        chi2_CR = self.chi2(phi_CR)
        chi2_DMCR = self.chi2(phi_DMCR)
        del_chi2 = -2 * np.log( 1/len(self.pp) * np.sum(np.exp((chi2_CR - chi2_DMCR)/2),axis=-1) )
        return del_chi2








    # # theta_prop - (number_of_m_DM,11) array containing generally accepteed values of propagation parameters 
    # def usual_theta_prop(number_of_m_DM,gamma_1p = 1.80, gamma_1 = 1.79, gamma_2p = 2.405, gamma_2 = 2.357, R_0 = 7.92e3, s = 0.37, D_0 = 2.05e28, delta = 0.419, v_alfven = 8.84, v_0c = 0.09, z_h = 2.60):
    #     theta_prop_temp = np.array([gamma_1p, gamma_1, gamma_2p, gamma_2, R_0, s, D_0, delta, v_alfven, v_0c, z_h])
    #     theta_prop = np.repeat([theta_prop_temp],number_of_m_DM,axis=0)
    #     return theta_prop 

    

    # # Check if given parameters are outside trained regions of the network
    
    # def outside_comfort_zone_DM(self,DM_masses):
    #     stop_DM = False
    #     #Checking DM parameters
    #     if np.min(DM_masses) < 5 or np.max(DM_masses) > 5e3:
    #         print('\n At least one of the given DM masses is outside of the provided range (5 GeV to 5 TeV). DM antiproton flux cannot be predicted.')
    #         stop_DM = True
    #     return stop_DM

    # def outside_comfort_zone_CR(self,propagation_parameters):
    #     stop_CR = False
    #     if propagation_parameters.shape[-1] != self.N_pp:
    #         raise ValueError('The number of propagation parameters is not consistent with the propagation model.')       
    #     #Checking propagation parameters
    #     strings = {'run1' : ['gamma 1,p', 'gamma 1', 'gamma 2,p', 'gamma 2', 'R_0', 's_0', 'D_0', 'delta', 'v_Alfven', 'v_0,c', 'z_h'],
    #                'DIFF.BRK' : ['gamma 2,p', 'gamma 2', 'D0xx', 'delta_l', 'delta', 'delta_h - delta', 'R_D0', 's_D', 'R_D1', 'v_0'],
    #                'INJ.BRK+vA': ['gamma 1,p', 'gamma 1', 'R_0,inj', 's', 'gamma 2,p', 'gamma 2', 'D_0', 'delta', 'delta_h - delta', 'R_1D', 'v_0', 'v_Alfven']} # TO DO: add
    #     for i in range(self.N_pp):
    #         if np.min(propagation_parameters[:, i]) <= self.mins_pp[i] or np.max(propagation_parameters[:, i]) >= self.maxs_pp[i]:
    #             print("At least one of the inputs for %s is outside the trained parameter ranges. No output will be given." % (strings[self.propagation_model])[i])
    #             print(np.min(self.pp[:, i]),np.max(self.pp[:, i]))
    #             stop_CR = True
    #             break
    #     return stop_CR

    # #br_fr - 2D array of shape (n,8), since 8 annihilation channels are included in the network. Note br_fr \in [1e-5,1)
    # def input_preprocessing_DM(self,DM_masses,br_fr,pp):
    #     # Min-max standardization of DM masses
    #     m_DM = ((np.log10(DM_masses)+3) - np.log10(5e3)) / (np.log10(5e6)-np.log10(5e3))
    #     # Replacing zeros by 1e-5
    #     bf_temp = np.where(br_fr<1e-5,1e-5,br_fr)
    #     # Normalizing branching fractions
    #     bf_temp = bf_temp / np.sum(bf_temp,axis=1)[:,None]
    #     # Processing braching fractions 
    #     bf = (np.log10(bf_temp) - np.array(self.DM_trafos[1,0])) / (np.array(self.DM_trafos[1,1])- np.array(self.DM_trafos[1,0]))
    #     # Processing propagation parameters
    #     theta_prop =  ((pp - np.array(self.DM_trafos[0,0]))/np.array(self.DM_trafos[0,1]))
    #     return m_DM,bf,theta_prop
    #     # Outputs processed 1D (m_DM) and 2D (bf,theta_prop)

    

    # sigma_v - (n,) array or scalar float containing cross section values 
    
        
    
