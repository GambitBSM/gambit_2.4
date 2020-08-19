import matplotlib.pyplot as plt
import h5py
import numpy as np
import sys

# all xsects from https://twiki.cern.ch/twiki/bin/view/LHCPhysics/SUSYCrossSections#SUSY_Cross_Sections_for_8_TeV_pp
xsect_100 = 5550.178  # 100 GeV chargino pair prod xsect
xsect_250 = 310.76  # 250 GeV chargino neutralino pair prod, from https://twiki.cern.ch/twiki/bin/view/CMSPublic/WinoCn
xsect_251 = 3.234  # 251 GeV slepton (selectron or smuon) pair prod, interpolated by assuming linear relation between 250 GeV and 255 GeV points
xsect_350 = 34.38103  # 350 GeV chargino pair prod

if len(sys.argv) < 2:
  print('You need to specify which plot to replicate!')
  print('Your options are: mll-3a, mll-3b, met-3c, met-3d, (SR-WWa)\nmt2-4a, mt2-4b, met-35a, met-35b (SR-mT2)\nmet-4c (SR-Zjets)')
  print('For the ones that are repeated (ex. met-3c and met-3d), the first is SF and second is DF.')
  #print('You also need to specify process: "SF" or "DF" (note that for plot 4c there is only SF)')
  exit()

plotname = sys.argv[1]
plot = plotname[-3:]
if plot == '4a' or plot == '35a':
  which_one = raw_input('Do you want slepton or chargino pair prod? s for slepton, c for chargino')

print('You are doing Figure ' + str(plot))
if plot == '-3a' or plot == '-3b' or plot == '-3c' or plot == '-3c':
  process = 4
elif (plot == '-4a' and which_one == 'c') or plot == '-4b' or (plot == '35a' and which_one == 'c') or plot == '35b':
  process = 2
elif (plot == '-4a' and which_one == 's') or (plot == '35a' and which_one == 's'):
  process = 1
elif plot '-4c':
  process = 3

normit = len(sys.argv) > 2
log_it = True

if plotname == "mll-3a":
  xsect = xsect_100
  var = 'mll'
  f = h5py.File('process'+process+'/ATLAS_8TeV_2LEPEW_20invfb_SF_SR-WWa_no-mll-cut.hdf5', 'r')
  dset = f[u'mll']
  print(dset[0,:])
  print(len(dset[0,:]))
  nbins = 12
  binwidth = 10.0
  pltrange = (20,140)
#  atlas_data = np.array([0,0.7,2.0,2.1,3.4,5.4,7.1,8.5,7.0,8.7,7.8,8.1,7.2,7.3,7.2,4.6,3.9,3.8,2.1,1.0])  # process 1, from my own observations
  atlas_data = np.array([0.1,0.3,1.9,2.0,3.3,5.3,7.2,8.4,6.9,8.5,7.6,8.1,6.8,7.2,7.0,4.4,3.6,3.5,1.9,0.8]) # using DataThief III
  title = r'Distribution (SF) of $m_{ll}$ after all SR-WWa cuts except $m_{ll}$ cuts, Process '+process
elif plotname == "mll-3b":
  xsect = xsect_100
  var = 'mll'
  f = h5py.File('process'+process+'/ATLAS_8TeV_2LEPEW_20invfb_DF_SR-WWa_no-mll-cut.hdf5', 'r')
  dset = f[u'mll']
  nbins = 12
  binwidth = 10.0
  pltrange = (20,140)
  atlas_data = np.array([7.2,8.5,7.0,8.7,7.7,8.1,6.9,7.2,7.1,4.6,3.8,3.6,2.0,0.4,0.3,0.1]) # using DataThief III
  title = r'Distribution (DF) of $m_{ll}$ after all SR-WWa cuts except $m_{ll}$ cut, Process '+process
elif plotname == "met-3c":
  xsect = xsect_100
  log_it = False
  var = 'MET'
  f = h5py.File('process'+process+'/ATLAS_8TeV_2LEPEW_20invfb_SF_SR-WWa_no-mll-met-cuts.hdf5', 'r')
  dset = f[u'met']
  nbins = 16
  binwidth = 10.0
  pltrange = (0,160)
  atlas_data = np.array([0, float('nan'), 3.8, 7.7 , 10.1, 9.5, 9.3, 9.7, 8.9, 6.5, 6.8, 6.2, 5.0, 5.2, 15.7])  # using DataThief III
  title = r'Distribution (SF) of $E_T^{miss, rel}$ after all SR-WWa cuts except $m_{ll}$ and $E_T^{miss, rel}$ cuts, Process '+process
elif plotname == "met-3d":
  xsect = xsect_100
  log_it = False
  var = 'MET'
  f = h5py.File('process'+process+'/ATLAS_8TeV_2LEPEW_20invfb_DF_SR-WWa_no-mll-met-cuts.hdf5', 'r')
  dset = f[u'met']
  nbins = 16
  binwidth = 10.0
  pltrange = (0,160)
  atlas_data = np.array([0, float('nan'), 3.8, 7.7 , 10.1, 9.5, 9.3, 9.7, 8.9, 6.5, 6.8, 6.2, 5.0, 5.2, 15.7])  # using DataThief III
  title = r'Distribution (DF) of $E_T^{miss, rel}$ after all SR-WWa cuts except $m_{ll}$ and $E_T^{miss, rel}$ cuts, Process '+process
elif plotname == "mt2-4a":
  if which_one=='s':
    xsect = xsect_251
  elif which_one=='c':
    xsect = xsect_350
  var = 'mT2'
  f = h5py.File('process'+process+'/ATLAS_8TeV_2LEPEW_20invfb_SF_SR-mT2_no-mt2-cut.hdf5', 'r')
#  f = h5py.File('process'+process+'/ATLAS_8TeV_2LEPEW_20invfb_SF_SR-WWa_no-mt2-cuts.hdf5', 'r') if which_one=='s' else h5py.File('process'+process+'/ATLAS_8TeV_2LEPEW_20invfb_SF_SR-WWa_no-mt2-cuts.hdf5')
  dset = f[u'mt2']
  nbins = 20
  binwidth = 10.0
  pltrange = (0,200)
  atlas_data = np.array([23.1,17.4,21.1,20.3,14.5,7.2,11.0,12.3,5.0,3.8,1.1,2.5,2.6,1.4,1.3,0.9,4.3]) if which_one=='s' else np.array([33.6,42.3,38.7,27.9,22.0,15.9,14.6,15.0,6.1,3.5,2.2,0.8,1.6,0.3,0.0,0.0,1.0])  # using DataThief III
  title = r'Distribution (SF) of $m_{T2}$ after all SR-$m_{T2}$ cuts except $m_{T2}$ cut, Process '+process
elif plotname == "mt2-4b":
  xsect = xsect_350
  var = 'mT2'
  f = h5py.File('process'+process+'/ATLAS_8TeV_2LEPEW_20invfb_DF_SR-mT2_no-mt2-cut.hdf5', 'r')
  dset = f[u'mt2']
  nbins = 20
  binwidth = 10.0
  pltrange = (0,200)
  atlas_data = np.array([23.1,17.4,21.1,20.3,14.5,7.2,11.0,12.3,5.0,3.8,1.1,2.5,2.6,1.4,1.3,0.9,4.3])  # using DataThief III
  title = r'Distribution (DF) of $m_{T2}$ after all SR-$m_{T2}$ cuts except $m_{T2}$ cut, Process '+process
elif plotname == "met-4c":
  xsect = xsect_250
  var = 'MET'
  f = h5py.File('process'+process+'/ATLAS_8TeV_2LEPEW_20invfb_SF_SR-Zjets_no-met-cut.hdf5', 'r')
  dset = f[u'met']
  nbins = 10
  binwidth = 20.0
  pltrange = (0,200)
  atlas_data = np.array([7.2,11.1,12.3,5.1,3.9,1.2,2.5,2.6,1.5,6.7])  # using DataThief III
  title = r'Distribution (SF) of $E_T^{miss, rel}$ after all SR-Zjets cuts except $E_T^{miss, rel}$ cut, Process '+process
elif plotname == "met-35a":
  if which_one=='s':
    xsect = xsect_251
  elif which_one=='c':
    xsect = xsect_350
  var = 'MET'
  f = h5py.File('process'+process+'/ATLAS_8TeV_2LEPEW_20invfb_SF_SR-mT2_no-mt2-cut.hdf5', 'r')
  dset = f[u'met']
  nbins = 20
  binwidth = 10.0
  pltrange = (0,200)
  atlas_data = np.array([23.1,17.4,21.1,20.3,14.5,7.2,11.0,12.3,5.0,3.8,1.1,2.5,2.6,1.4,1.3,0.9,4.3]) if which_one=='s' else np.array([33.6,42.3,38.7,27.9,22.0,15.9,14.6,15.0,6.1,3.5,2.2,0.8,1.6,0.3,0.0,0.0,1.0])  # using DataThief III
  title = r'Distribution (SF) of $E_T^{miss, rel}$ after all SR-$m_{T2}$ cuts except $m_{T2}$ cut, Process '+process
elif plotname == "met-35b":
  xsect = xsect_350
  var = 'MET'
  f = h5py.File('process'+process+'/ATLAS_8TeV_2LEPEW_20invfb_DF_SR-mT2_no-mt2-cut.hdf5', 'r')
  dset = f[u'met']
  nbins = 20
  binwidth = 10.0
  pltrange = (0,200)
  atlas_data = np.array([23.1,17.4,21.1,20.3,14.5,7.2,11.0,12.3,5.0,3.8,1.1,2.5,2.6,1.4,1.3,0.9,4.3])  # using DataThief III
  title = r'Distribution (DF) of $E_T^{miss, rel}$ after all SR-$m_{T2}$ cuts except $m_{T2}$ cut, Process '+process
else:
  print('You need to specify which plot to replicate!\nYour options are: mll-3a, mll-3b, met-3c, met-3d, (SR-WWa)\nmt2-4a, mt2-4b, met-35a, met-35b (SR-mT2)\nmet-4c (SR-Zjets)')
  exit()


bin_centers = (np.linspace(*pltrange, num=nbins+1) + binwidth/2)[:-1]

vals = dset[0,:]
#factor = 1.0/binwidth
#factor = (1.0/binwidth)*((xsect*20.3)/50000.0)
factor = ((xsect*20.3)/400000.0)
if normit:
  plt.hist(np.clip(vals,*pltrange), bins=nbins,range=pltrange, weights=factor*np.ones_like(vals), histtype='step', color='blue', label='CBS', density=True)
  plt.hist(bin_centers,bins=nbins, range=pltrange, weights=atlas_data, histtype='step',color='black',label='ATLAS', density=True)
else:
  plt.hist(np.clip(vals,*pltrange), bins=nbins,range=pltrange, weights=factor*np.ones_like(vals), histtype='step', color='blue', label='CBS')
  plt.hist(bin_centers,bins=nbins, range=pltrange, weights=atlas_data, histtype='step',color='black',label='ATLAS')
  
  atlas_uncert = 0.4  # width of line on plot is 0.7
  #atlas_uncert = 0.07  # cursor jumps by a minimum of 0.07 on the y axis wait never mind I just zoomed in more
  atlas_uncert_err = np.concatenate(([0], atlas_uncert*np.ones_like(bin_centers), [0]), axis=0)
  bin_centers_err = np.concatenate(([pltrange[0]-binwidth/2],bin_centers,[pltrange[1]+binwidth/2]), axis=0)
  atlas_data_err = np.concatenate(([0],atlas_data,[0]), axis=0)
  plt.errorbar(bin_centers_err, atlas_data_err, yerr=atlas_uncert_err, marker='', ls='', drawstyle='steps-mid', color='black')

plt.xlabel("{} [GeV]".format(var))
#plt.ylabel("Events / {} GeV".format(int(binwidth)))
plt.ylabel("Events (Bin Width {} GeV)".format(int(binwidth)))
plt.legend()
if normit:
  plt.title('(NORM) ' + title)
  plt.savefig(plotname+'-pr'+str(process)+'-norm'+'.png',dpi=250)
else:
  plt.title(title)
  plt.savefig(plotname+'-pr'+str(process)+'.png',dpi=250)

if log_it:
  plt.yscale("log")

plt.show()


