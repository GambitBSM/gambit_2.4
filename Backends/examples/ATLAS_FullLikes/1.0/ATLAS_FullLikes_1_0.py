#  \file
#
#  ATLAS full likelihoods with pyhf
#
#  *************************************
#
#  \author Chris Chang
#          (christopher.chang@uqconnect.edu.au)
#  \date 2021
#
#  **************************************

"""
This takes the signal data that will be output by ColliderBit and patches the 
background data. It then uses pyhf to find the LogLike. Python 3.6 or greater required (needed for pyhf more recent versions).
Required:
  - Signal data per signal region
  - Background only data json file
  - python >= v3.6 + pyhf
Outputs:
  - Loglike
Current Tasks:
  - Running prevents GAMBIT from finishing after the calling MPI finalise step (occasionally seg faults)
  - To test requires the Analysis: ATLAS_13TeV_1Lep2b_139infb valdiated
  - Calculating the loglike gives a pyhf Runtime warning about an invalid value inside of the calculation (may be unavoidable)
  - Need to throw error if trying to call analysis that isn't implemented
  - Fix author sections on all ATLAS FullLikes code (mostly left to what copied over)
"""

import pyhf as pyhf
import json
import jsonpatch
import jsonschema
import numpy as np
pyhf.set_backend("numpy")

# TODO: Will this as a global work?
# workspace = None
ws = None
channelsample_dict = None
Nbindict = None

# TODO: This function (don't forget to connect to GB) should occur at ColliderBit initialisation, and should save the bkg files (perhaps as one dictionary object), and return an error code if fail to open (so that the non full-likelihood can be run instead)
def ReadJsonFiles(AnalysisName): #TODO: Print statements?
    # global workspace
    global ws
    global channelsample_dict
    global Nbindict
    try:
      with open('Backends/examples/ATLAS_FullLikes/1.0/Analyses/' + AnalysisName + '/BkgOnly.json','r') as bkg:
        workspace = json.load(bkg) #TODO: Will this path work in general?

        # Finding the corresponding Data entry for a given signal region.
        i = 0
        channelsample_dict = {}
        for channel in workspace["channels"]:
          channelsample_dict[channel["name"]] = (i,len(channel["samples"]))
          i = i+1

      Nbindict = {} #TODO: Combine this info with the above dictionary into a singular information dictionary
      for channel in workspace["observations"]:
        Nbindict[channel["name"]] = len(channel["data"])

      with open('Backends/examples/ATLAS_FullLikes/1.0/workspace.json','r') as wk:
        schema = json.load(wk)

      # Validate the workspace against a scheme
      jsonschema.validate(instance=workspace, schema=schema)
      ws = pyhf.Workspace(workspace)

    except:
      return(1)

    return(0)

def FullLikeBackend(mydict):
  # Creating the json patch #TODO: Reasonable things to put in the for metadata
  data = {}
  data['metadata'] = {
      "description": "signal patchset for the SUSY EWK 1Lbb analysis",
      "digests": {
              "sha256": "2563672e1a165384faf49f1431e921d88c9c07ec10f150d5045576564f98f820"
          },
      "labels": [
              "comenergy" #TODO: The scheme requires some label and value (added below). But we don't actually need to use it. I will try think of something reasonable.
          ],
          "references": {
              "hepdata": "ins1755298"
          }
      }

  data['patches'] = []
  data["version"] = "1.0.0"

  Signal = {}
      
  # Add in the signal data to a patch
  for key,value in channelsample_dict.items():
    Signal[str(value[0])+"_"+str(value[1])] = []
    for i in range(Nbindict[key]):
      Signal[str(value[0])+"_"+str(value[1])].append(mydict[key+"_"+str(i)])

  data['patches'].append({
          "metadata":
              {
              "name": "SignalData",
              "values": [13.0]
              },})
  
  data['patches'][0]['patch'] = []
    
  # Appending for each separate path
  for key,value in channelsample_dict.items():
    data['patches'][0]['patch'].append(
              {
                  "op": "add",
                  "path": "/channels/{0}/samples/{1}".format(value[0],value[1]),
                  "value": {
                      "data": 
                          Signal["{0}_{1}".format(value[0],value[1])]
                      ,
                      "modifiers": [
                          {
                              "data": None,
                              "name": "mu_SIG",
                              "type": "normfactor"
                          }
                      ],
                      "name": "SignalData"
                  }
              },)

  # Patching the background data and setting the model
  PatchJson = pyhf.patchset.PatchSet(data)
  model = ws.model(patches=PatchJson)
  bestfit_pars, twice_nll = pyhf.infer.mle.fit(ws.data(model),model, return_fitted_val=True)

  return(-0.5*twice_nll)

#TODO: This is the version without a different function for reading in at the start
"""
def FullLikeBackend(mydict,AnalysisName):
  # Pulling in the Background only data and validating it against the scheme. TODO: This may need to only occur once at th start of a scan.

  #TODO: Check that this works for multiple Analyses.
  with open('Backends/examples/ATLAS_FullLikes/1.0/Analyses/' + AnalysisName + '/BkgOnly.json') as bkg:
    with open('Backends/examples/ATLAS_FullLikes/1.0/workspace.json') as wk:
      workspace = json.load(bkg) #TODO: Will this path work in general?

      # Finding the corresponding Data entry for a given signal region.
      i = 0
      channelsample_dict = {}
      for channel in workspace["channels"]:
        channelsample_dict[channel["name"]] = (i,len(channel["samples"]))
        i = i+1

      Nbindict = {} #TODO: Combine this info with the above dictionary into a singular information dictionary
      for channel in workspace["observations"]:
        Nbindict[channel["name"]] = len(channel["data"])

      # Validate the workspace against a scheme
      schema = json.load(wk)
      jsonschema.validate(instance=workspace, schema=schema)
      ws = pyhf.Workspace(workspace)

      # TODO: Remove Comment. Testing making less in the opened file area
  # Creating the json patch #TODO: Reasonable things to put in the for metadata
  data = {}
  data['metadata'] = {
      "description": "signal patchset for the SUSY EWK 1Lbb analysis",
      "digests": {
              "sha256": "2563672e1a165384faf49f1431e921d88c9c07ec10f150d5045576564f98f820"
          },
      "labels": [
              "comenergy" #TODO: The scheme requires some label and value (added below). But we don't actually need to use it. I will try think of something reasonable.
          ],
          "references": {
              "hepdata": "ins1755298"
          }
      }

  data['patches'] = []
  data["version"] = "1.0.0"

  Signal = {}
      
  # Add in the signal data to a patch
  for key,value in channelsample_dict.items():
    Signal[str(value[0])+"_"+str(value[1])] = []
    for i in range(Nbindict[key]):
      Signal[str(value[0])+"_"+str(value[1])].append(mydict[key+"_"+str(i)])

  data['patches'].append({
          "metadata":
              {
              "name": "SignalData",
              "values": [13.0]
              },})
  
  data['patches'][0]['patch'] = []
    
  # Appending for each separate path
  for key,value in channelsample_dict.items():
    data['patches'][0]['patch'].append(
              {
                  "op": "add",
                  "path": "/channels/{0}/samples/{1}".format(value[0],value[1]),
                  "value": {
                      "data": 
                          Signal["{0}_{1}".format(value[0],value[1])]
                      ,
                      "modifiers": [
                          {
                              "data": None,
                              "name": "mu_SIG",
                              "type": "normfactor"
                          }
                      ],
                      "name": "SignalData"
                  }
              },)

  # Patching the background data and setting the model
  PatchJson = pyhf.patchset.PatchSet(data)
  model = ws.model(patches=PatchJson)
  bestfit_pars, twice_nll = pyhf.infer.mle.fit(ws.data(model),model, return_fitted_val=True)

  return(-0.5*twice_nll)
"""
