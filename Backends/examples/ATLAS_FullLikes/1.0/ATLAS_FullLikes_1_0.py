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
  - Loglike (s+b) - Loglike (b) 
todo:
  - Calculating the loglike gives a pyhf Runtime warning about an invalid value inside of the calculation (may be unavoidable)
"""

import pyhf as pyhf
import json
import jsonpatch
import jsonschema
import numpy as np
pyhf.set_backend("numpy")

ws = None
channelsample_dict = None
Nbindict = None

def ReadJsonFiles(AnalysisName):
    # global workspace
    global ws
    global channelsample_dict
    global Nbindict
    try:
      with open('Backends/examples/ATLAS_FullLikes/1.0/Analyses/' + AnalysisName + '/BkgOnly.json','r') as bkg:
        workspace = json.load(bkg)

        # Finding the corresponding Data entry for a given signal region.
        i = 0
        channelsample_dict = {}
        for channel in workspace["channels"]:
          channelsample_dict[channel["name"]] = (i,len(channel["samples"]))
          i = i+1

      Nbindict = {}
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
  # Creating the json patch
  data = {}
  
  # metadata must meet requirement in json scheme
  data['metadata'] = {
      "description": "signal patchset for ColliderBit Analysis",
      "digests": {
              "sha256": "2563672e1a165384faf49f1431e921d88c9c07ec10f150d5045576564f98f820"
          },
      "labels": [
              "comenergy"
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
      #TODO Debugging prints
      print(key+"_"+str(i))
      print(mydict[key+"_"+str(i)])
      Signal[str(value[0])+"_"+str(value[1])].append(mydict[key+"_"+str(i)])

  data['patches'].append({
          "metadata":
              {
              "name": "SignalData",
              "values": [13.0]
              },})
  

  data['patches'][0]['patch'] = []

  # First calculating the LogLike for background only, by adding in a patch with a data entry of zero.
  for key,value in channelsample_dict.items():
    BKG = []
    for i in range(Nbindict[key]):
      BKG.append(0.0)
  
    data['patches'][0]['patch'].append(
              {
                  "op": "add",
                  "path": "/channels/{0}/samples/{1}".format(value[0],value[1]),
                  "value": {
                      "data": 
                          BKG
                      ,
                      "modifiers": [
                          {
                              "data": None,
                              "name": "mu_SIG",
                              "type": "normfactor"
                          },
                          {
                              "data": None,
                              "name": "lumi",
                              "type": "lumi"
                          }                          
                      ],
                      "name": "Zero for calculating BKG likelihood"
                  }
              },)
  PatchJson = pyhf.patchset.PatchSet(data)
  model = ws.model(patches=PatchJson)
  bestfit_pars, twice_nll_b = pyhf.infer.mle.fixed_poi_fit(1.,ws.data(model),model, return_fitted_val=True)
  
  # Appending for each separate path
  for key,value in channelsample_dict.items():
    data['patches'][0]['patch'].append(
              {
                  "op": "add",
                  "path": "/channels/{0}/samples/{1}".format(value[0],value[1]+1),
                  "value": {
                      "data": 
                          Signal["{0}_{1}".format(value[0],value[1])]
                      ,
                      "modifiers": [
                          {
                              "data": None,
                              "name": "mu_SIG",
                              "type": "normfactor"
                          },
                          {
                              "data": None,
                              "name": "lumi",
                              "type": "lumi"
                          } 
                      ],
                      "name": "SignalData"
                  }
              },)

  # Patching the background data and setting the model
  PatchJson = pyhf.patchset.PatchSet(data)
  model = ws.model(patches=PatchJson)
  bestfit_pars, twice_nll_sb = pyhf.infer.mle.fixed_poi_fit(1.,ws.data(model),model, return_fitted_val=True)

  dll = (-0.5*twice_nll_sb) - (-0.5*twice_nll_b)

  return(dll)

