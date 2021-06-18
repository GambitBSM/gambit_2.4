"""
Script to read in the data files, and interpolate the cross-sections.

Author: Christopher Chang
Date: May 2020

The base data (to reference) is provided at https://twiki.cern.ch/twiki/bin/view/LHCPhysics/SUSYCrossSections

"""

from scipy import interpolate
import re
import os
import sys
import numpy as np

#Finding the absolute path to this file
my_path = os.path.dirname(os.path.abspath(__file__))

#Global dictionary to store the interpolated functions
functiondict = {}

# #Global dictionary to store info on the data tables
# datfiles = {}


#
# A minimal container class to hold the cross-section functions 
# for a given PID pair, plus other relevant info
#
class XsecFuncContainer:
    def __init__(self, PID_pair, energy, degeneracy, mass_range, xsec_function, unc_function, data_file_name):
        self.PID_pair = PID_pair
        self.energy = energy
        self.degeneracy = degeneracy
        self.mass_range = mass_range
        self.xsec_function = xsec_function
        self.unc_function = unc_function
        self.data_file_name = data_file_name


#
# Helper function that takes input like this string
#
#   "1000022_1000022;1000022_1000023;-1000024_1000022;1000024_1000022;"
#
# and returns a list of (internally sorted) PID pairs like this:
#
#   [(1000022,1000022), (1000022,1000023), (-1000024,1000022), (1000022,1000024)] 
#
def str_to_PID_pair_list(input_str):
    result = []
    s = input_str
    s = s.rstrip(";")
    for pair_str in s.split(";"):
        each_PID_str_list = pair_str.split("_")
        PID_pair = tuple( sorted( [ int(each_PID_str_list[0]), int(each_PID_str_list[1]) ] ) )
        result.append(PID_pair)
    return result


#
# Initialisation function that reads in the data and forms the interpolation function.
#
def init(simplexs_init_pars):

    print("DEBUG: Got these settings from ColliderBit: ", simplexs_init_pars, flush=True)

    # This function will edit the following global variables
    global functiondict

    # Now let's fill the functiondict with sub-dictionaries as per this example: 
    # functiondict = 
    # {
    #     8: 
    #         {
    #             (1000022,1000022): <XsecFuncContainer for (1000022,1000022) at 8 TeV>,
    #             (1000022,1000023): <XsecFuncContainer for (1000022,1000023) at 8 TeV>,
    #             <etc.>
    #         },
    #     13: 
    #         {
    #             (1000022,1000022): <XsecFuncContainer for (1000022,1000022) at 13 TeV>,
    #             (1000022,1000023): <XsecFuncContainer for (1000022,1000023) at 13 TeV>,
    #             <etc.>
    #         },
    #     <etc.>
    # }
    # 
    # Then we can later access the XsecFuncContainer for a given energy and PID pair like this: 
    #   
    #   functdict[13][[1000022,1000023]]
    #

    for datfile, init_subdict in simplexs_init_pars.items():

        energy = int(init_subdict["energy_TeV"])
        degeneracy = init_subdict["degeneracy_factor"]

        if energy not in functiondict:
            functiondict[energy] = {}

        PID_pairs = str_to_PID_pair_list(init_subdict["use_for_PID_pairs"])

        for PID_pair in PID_pairs:

            # Check that we don't have a function container for this PID pair and energy already:
            if PID_pair in functiondict[energy]:
                old_xsfc = functiondict[energy][PID_pair]
                error_msg = "simple_xs error: A cross-section function has already been constructed for PID pair (%i,%i) and energy (%i TeV). The existing function is based on data file %s." % (PID_pair[0], PID_pair[1], energy, old_xsfc.data_file_name)
                raise Exception(error_msg)

            #Opening file and reading the mass, xsec and uncertainty
            with open(os.path.join(my_path, "Data/" + datfile),"r") as f:
                data = np.loadtxt(f)
            
            # data[:,0]=Masses (GeV), data[:,1]=xsec and data[:,2]=absolute uncertainty in xsec
            xs_interp_func = interpolate.interp1d(data[:,0],data[:,1])
            unc_interp_func = interpolate.interp1d(data[:,0],data[:,2])
            mass_range = [data[-1,0], data[0,0]]

            # Now we have everything we need to construct the XsecFuncContainer instance 
            # and save it in functiondict
            functiondict[energy][PID_pair] = XsecFuncContainer(PID_pair, energy, degeneracy, mass_range, xs_interp_func, unc_interp_func, datfile)


#
# Read in the PID pair and energy (TeV), and form the file path to the corresponding data file
#
def get_xsection(simplexs_pars, masses_GeV):

    #Reading in the entries
    PID_pair = tuple( sorted( simplexs_pars["proc"] ) )  # Note: this must be a tuple to be used as a dictionary key
    comenergy = simplexs_pars["energy_TeV"]

    # Since all we have are 1D cross-section functions for degenerate masses, 
    # just find the mass corresponding to the first PID in the PID_pair.
    # (Need to use masses for both PIDs if we start doing cross-sections for non-degenerate cases.)
    mass_key = str( abs(PID_pair[0]) )
    mass = masses_GeV[mass_key]
    
    # Raise an error if we haven't prepared a cross-section function for the given comenergy and PID pair
    assert int(comenergy) in functiondict, "simple_xs error: Supplied COM energy (%i TeV) does not match any of the data files." % (comenergy)
    assert PID_pair in functiondict[comenergy], "simple_xs error: Supplied PID pair (%i,%i) cannot be used with any of the data files." % (PID_pair[0], PID_pair[1])

    # Get the XsecFuncContainer instance
    xsfc = functiondict[comenergy][PID_pair]

    #Raise error if mass is outside of interpolation range
    assert (mass < xsfc.mass_range[0]) and (mass > xsfc.mass_range[1]), "simple_xs error: Supplied mass is outside of interpolation range. Supplied mass: %0.1f. Acceptable mass range: (%0.1f,%0.1f)"%(mass, xsfc.mass_range[0], xsfc.mass_range[1])

    #Assigning result          
    result_dict = {}
    result_dict["central"] = xsfc.xsec_function(mass) / xsfc.degeneracy  
    #Since the uncertainty functions give the relative uncertainties, 
    #we must multiply by the central cross-section value
    result_dict["tot_err_down"] = xsfc.unc_function(mass) * result_dict["central"]
    result_dict["tot_err_up"]   = result_dict["tot_err_down"]  

    return result_dict

