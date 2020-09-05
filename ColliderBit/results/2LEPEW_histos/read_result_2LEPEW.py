import matplotlib.pyplot as plt
import h5py
import numpy as np
import sys

# Plotting script for histograms generating by enabling Perf_Plot features in Analysis_ATLAS_8TeV_2LEPEW_20invfb.cpp
# Code by Kelton Whiteaker
# For now, no support for the CR histograms (Figures 2a,b,c,d)

# all xsects from https://twiki.cern.ch/twiki/bin/view/LHCPhysics/SUSYCrossSections#SUSY_Cross_Sections_for_8_TeV_pp
xsect_100 = 5550.178  # 100 GeV chargino pair prod xsect
xsect_250 = 310.76  # 250 GeV chargino neutralino pair prod, from https://twiki.cern.ch/twiki/bin/view/CMSPublic/WinoCn
xsect_251 = 3.234  # 251 GeV slepton (selectron or smuon) pair prod, interpolated by assuming linear relation between 250 GeV and 255 GeV points
xsect_350 = 34.38103  # 350 GeV chargino pair prod

if len(sys.argv) < 2:
  print('You need to specify which plot to replicate!')
  print('Your options are: mll-3a, mll-3b, met-3c, met-3d, (SR-WWa)\nmt2-4a (pr1 or 2), mt2-4b, met-35a (pr1 or 2), met-35b (SR-mT2)\nmet-4c (SR-Zjets)')
  print('You can normalize both histos to integrate to 1 by providing a 3rd argument')
  #print('For the variables that are repeated within the same figure (ex. met-3c and met-3d), the first is SF and second is DF.')
  exit()

plotname = sys.argv[1]
plot = plotname[-3:]
if plot == '-4a' or plot == '35a':
  which_one = raw_input('Do you want slepton (pr1) or chargino (pr2) pair prod? s for slepton, c for chargino: ')

print('You are doing Figure ' + str(plot))
if plot == '-3a' or plot == '-3b' or plot == '-3c' or plot == '-3d':
  process = '4'
elif (plot == '-4a' and which_one == 'c') or plot == '-4b' or (plot == '35a' and which_one == 'c') or plot == '35b':
  process = '2'
elif (plot == '-4a' and which_one == 's') or (plot == '35a' and which_one == 's'):
  process = '1'
elif plot == '-4c':
  process = '3'

normit = len(sys.argv) > 2  # normalize so both histos integrate to 1
log_it = True  # is the histo on log vertical scale?

if plotname == "mll-3a":
  xsect = xsect_100
  var = 'mll'
  f = h5py.File('process'+process+'/ATLAS_8TeV_2LEPEW_20invfb_SF_SR-WWa_no-mll-cut.hdf5', 'r')
  dset = f[u'mll']
#  print(dset[0,:])  # for debugging
#  print(len(dset[0,:]))
  nbins = 12
  binwidth = 10.0
  pltrange = (20,140)
  atlas_data = np.array([1.9,2.7,2.6,2.8,3.6,4.0,3.1,2.6,2.9,3.2,2.3,11.8]) # using DataThief III
  title = r'Distribution (SF) of $m_{ll}$ after all SR-WWa cuts,'+'\n'+r'except $m_{ll}$ cuts, Process '+process
elif plotname == "mll-3b":
  xsect = xsect_100
  var = 'mll'
  f = h5py.File('process'+process+'/ATLAS_8TeV_2LEPEW_20invfb_DF_SR-WWa_no-mll-cut.hdf5', 'r')
  dset = f[u'mll']
  nbins = 12
  binwidth = 10.0
  pltrange = (20,140)
  atlas_data = np.array([2.0,2.3,3.4,3.0,3.2,2.4,2.6,2.6,2.4,3.3,1.6,11.5]) # using DataThief III
  title = r'Distribution (DF) of $m_{ll}$ after all SR-WWa cuts,'+'\n'+r'except $m_{ll}$ cut, Process '+process
elif plotname == "met-3c":
  xsect = xsect_100
  log_it = False
  var = 'MET'
  f = h5py.File('process'+process+'/ATLAS_8TeV_2LEPEW_20invfb_SF_SR-WWa_no-mll-met-cuts.hdf5', 'r')
  dset = f[u'met']
  nbins = 17
  binwidth = 10.0
  pltrange = (0,170)
  atlas_data = np.array([1.1,1.6,1.2,1.4,2.3,3.3,6.3,9.8,11.4,10.6,8.5,4.7,3.2,1.8,0.9,0.5,0.8])  # using DataThief III
  title = r'Distribution (SF) of $E_T^{miss, rel}$ after all SR-WWa cuts,'+'\n'+r'except $m_{ll}$ and $E_T^{miss, rel}$ cuts, Process '+process
elif plotname == "met-3d":
  xsect = xsect_100
  log_it = False
  var = 'MET'
  f = h5py.File('process'+process+'/ATLAS_8TeV_2LEPEW_20invfb_DF_SR-WWa_no-mll-met-cuts.hdf5', 'r')
  dset = f[u'met']
  nbins = 17
  binwidth = 10.0
  pltrange = (0,170)
  atlas_data = np.array([0.9,1.7,1.3,2.4,3.0,3.8,5.7,8.6,11.9,9.5,7.7,4.8,2.6,1.8,0.7,0.1,0.6])  # using DataThief III
  title = r'Distribution (DF) of $E_T^{miss, rel}$ after all SR-WWa cuts,'+'\n'+r'except $m_{ll}$ and $E_T^{miss, rel}$ cuts, Process '+process
elif plotname == "mt2-4a":
  if which_one=='s':  # in this case the events were generated in two versions: ee or mumu. Must combine these. I named them _ee and _mumu by hand.
    xsect = xsect_251
    f1 = h5py.File('process'+process+'/ATLAS_8TeV_2LEPEW_20invfb_SF_SR-mT2_ee_no-mt2-cut.hdf5', 'r')
    f2 = h5py.File('process'+process+'/ATLAS_8TeV_2LEPEW_20invfb_SF_SR-mT2_mumu_no-mt2-cut.hdf5', 'r')
    dset1 = np.array(f1[u'mt2'])
    dset2 = np.array(f2[u'mt2'])
    dset = np.concatenate((dset1,dset2),axis=1)  # this is me combining the events
  elif which_one=='c':
    xsect = xsect_350
    f = h5py.File('process'+process+'/ATLAS_8TeV_2LEPEW_20invfb_SF_SR-mT2_no-mt2-cut.hdf5', 'r')
    dset = f[u'mt2']
  var = 'mT2'
  nbins = 20
  binwidth = 10.0
  pltrange = (0,200)
  atlas_data = np.array([2.0,1.2,2.2,1.3,1.8,1.4,1.3,1.5,1.2,1.1,1.7,2.0,2.0,1.3,1.4,1.5,1.4,2.4,1.7,6.8]) if which_one=='s' else np.array([8.9,1.4,1.4,1.5,1.8,2.0,1.5,2.3,2.3,2.2,2.0,1.8,2.1,2.0,2.0,1.7,1.3,1.2,1.2,6.2])  # using DataThief III
  title = r'Distribution (SF) of $m_{T2}$ after all SR-$m_{T2}$ cuts,'+'\n'+r'except $m_{T2}$ cut, Process '+process
elif plotname == "mt2-4b":
  xsect = xsect_350
  var = 'mT2'
  f = h5py.File('process'+process+'/ATLAS_8TeV_2LEPEW_20invfb_DF_SR-mT2_no-mt2-cut.hdf5', 'r')
  dset = f[u'mt2']
  nbins = 20
  binwidth = 10.0
  pltrange = (0,200)
  atlas_data = np.array([1.7,1.4,1.4,1.4,1.6,1.5,1.3,1.4,1.6,1.4,1.4,1.5,1.6,1.7,1.3,1.5,1.3,1.4,1.0,4.6])  # using DataThief III
  title = r'Distribution (DF) of $m_{T2}$ after all SR-$m_{T2}$ cuts,'+'\n'+r'except $m_{T2}$ cut, Process '+process
elif plotname == "met-4c":
  xsect = xsect_250
  var = 'MET'
  f = h5py.File('process'+process+'/ATLAS_8TeV_2LEPEW_20invfb_SF_SR-Zjets_no-met-cut.hdf5', 'r')
  dset = f[u'met']
  nbins = 20
  binwidth = 10.0
  pltrange = (0,200)
  atlas_data = np.array([1.4,1.6,0.9,0.9,0.9,0.6,0.8,0.6,0.8,0.6,0.5,0.5,0.6,0.7,0.2,0.4,0.5,0.2,0.2,1.1])  # using DataThief III
  title = r'Distribution (SF) of $E_T^{miss, rel}$ after all SR-Zjets cuts,'+'\n'+r'except $E_T^{miss, rel}$ cut, Process '+process
elif plotname == "met-35a":
  if which_one=='s': # in this case the events were generated in two versions: ee or mumu. Must combine these. I named them _ee and _mumu by hand.
    xsect = xsect_251
    f1 = h5py.File('process'+process+'/ATLAS_8TeV_2LEPEW_20invfb_SF_SR-mT2_ee_no-mt2-cut.hdf5', 'r')
    f2 = h5py.File('process'+process+'/ATLAS_8TeV_2LEPEW_20invfb_SF_SR-mT2_mumu_no-mt2-cut.hdf5', 'r')
    dset1 = np.array(f1[u'met'])
    dset2 = np.array(f2[u'met'])
    dset = np.concatenate((dset1,dset2),axis=1)  # this is me combining the events
  elif which_one=='c':
    xsect = xsect_350
    f = h5py.File('process'+process+'/ATLAS_8TeV_2LEPEW_20invfb_SF_SR-mT2_no-mt2-cut.hdf5', 'r')
    dset = f[u'met']
  var = 'MET'
  nbins = 20
  binwidth = 10.0
  pltrange = (0,200)
  atlas_data = np.array([1.0,1.3,1.4,1.2,1.5,1.0,1.2,1.3,1.0,1.5,0.8,1.5,1.7,1.6,1.5,1.3,1.2,1.2,1.1,13.0]) if which_one=='s' else np.array([1.1,1.3,1.1,1.0,1.3,1.6,1.5,1.4,2.1,1.5,2.0,1.6,1.9,1.7,2.4,1.6,1.5,1.4,1.6,10.0])  # using DataThief III
  title = r'Distribution (SF) of $E_T^{miss, rel}$ after all SR-$m_{T2}$ cuts,'+'\n'+r'except $m_{T2}$ cut, Process '+process
elif plotname == "met-35b":
  xsect = xsect_350
  var = 'MET'
  f = h5py.File('process'+process+'/ATLAS_8TeV_2LEPEW_20invfb_DF_SR-mT2_no-mt2-cut.hdf5', 'r')
  dset = f[u'met']
  nbins = 20
  binwidth = 10.0
  pltrange = (0,200)
  atlas_data = np.array([0.9,1.3,1.1,1.3,1.1,1.1,1.3,1.2,1.2,1.3,1.2,1.4,1.5,1.3,1.5,1.4,1.3,1.4,1.2,7.8])  # using DataThief III
  title = r'Distribution (DF) of $E_T^{miss, rel}$ after all SR-$m_{T2}$ cuts,'+'\n'+r'except $m_{T2}$ cut, Process '+process
else:
  print('You need to specify which plot to replicate!\nYour options are: mll-3a, mll-3b, met-3c, met-3d, (SR-WWa)\nmt2-4a, mt2-4b, met-35a, met-35b (SR-mT2)\nmet-4c (SR-Zjets)')
  exit()


bin_centers = (np.linspace(*pltrange, num=nbins+1) + binwidth/2)[:-1]

vals = dset[0,:]
#print(vals)  # for debugging
evtnum = 400000.0  # number of generated events
factor = ((xsect*20.3)/evtnum)  # need to normalize event number as (we assume) ATLAS did, so that our histo matches theirs
if normit:
  plt.hist(np.clip(vals,*pltrange), bins=nbins,range=pltrange, weights=factor*np.ones_like(vals), histtype='step', color='blue', label='CBS', density=True)
  plt.hist(bin_centers,bins=nbins, range=pltrange, weights=atlas_data, histtype='step',color='black',label='ATLAS', density=True)
else:
  plt.hist(np.clip(vals,*pltrange), bins=nbins,range=pltrange, weights=factor*np.ones_like(vals), histtype='step', color='blue', label='CBS')
  plt.hist(bin_centers,bins=nbins, range=pltrange, weights=atlas_data, histtype='step',color='black',label='ATLAS')
  
#  atlas_uncert = 0.4  # width of line on plot is 0.7 events on y axis
  atlas_uncert = 0.0  # line width varies per plot, and per data point if plot is on a log scale, so I'm just making it zero and telling you to keep in mind my data is read off the published histo so isn't perfect
  atlas_uncert_err = np.concatenate(([0], atlas_uncert*np.ones_like(bin_centers), [0]), axis=0)
  bin_centers_err = np.concatenate(([pltrange[0]-binwidth/2],bin_centers,[pltrange[1]+binwidth/2]), axis=0)
  atlas_data_err = np.concatenate(([0],atlas_data,[0]), axis=0)
  plt.errorbar(bin_centers_err, atlas_data_err, yerr=atlas_uncert_err, marker='', ls='', drawstyle='steps-mid', color='black')

plt.xlabel("{} [GeV]".format(var))
plt.ylabel("Events (Bin Width {} GeV)".format(int(binwidth)))
plt.legend()
if normit:
  plt.title('(NORM) ' + title)
  plt.savefig(plotname+'-pr'+process+'-norm'+'.png',dpi=250)
else:
  plt.title(title)
  plt.savefig(plotname+'-pr'+process+'.png',dpi=250)

if log_it:
  plt.yscale("log")

plt.show()


