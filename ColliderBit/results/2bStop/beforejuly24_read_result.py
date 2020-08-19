import matplotlib.pyplot as plt
import h5py
import numpy as np
import sys



plotname = sys.argv[1]
process = sys.argv[2]
normit = len(sys.argv) > 3
if plotname == "mct-3a":
  var = 'mCT'
  f = h5py.File('process'+process+'/ATLAS_8TeV_2bStop_20invfb_SRA_no-mct-cut.hdf5', 'r')
  dset = f[u'mct']
  nbins = 20
  binwidth = 25.0
  pltrange = (0,500)
#  atlas_data = np.array([0,0.7,2.0,2.1,3.4,5.4,7.1,8.5,7.0,8.7,7.8,8.1,7.2,7.3,7.2,4.6,3.9,3.8,2.1,1.0])  # process 1, from my own observations
  atlas_data = np.array([0.1,0.3,1.9,2.0,3.3,5.3,7.2,8.4,6.9,8.5,7.6,8.1,6.8,7.2,7.0,4.4,3.6,3.5,1.9,0.8]) if process=='1' else np.array([0,0.3,1.5,1.6,2.6,4.8,7.4,7.0,7.8,7.4,10.1,7.2,5.6,4.2,4.4,3.5,1.7,1.2,0.7,0.0])  # using DataThief III
  title = r'Distribution of $m_{CT}$ after all SRA cuts except $m_{CT}$ cuts, Process '+process
elif plotname == "mct-aux2a":
  var = 'mCT'
  f = h5py.File('process'+process+'/ATLAS_8TeV_2bStop_20invfb_SRA_no-mct-cut.hdf5', 'r')
  dset = f[u'mct']
  nbins = 16
  binwidth = 25.0
  pltrange = (150,550)
  atlas_data = np.array([7.2,8.5,7.0,8.7,7.7,8.1,6.9,7.2,7.1,4.6,3.8,3.6,2.0,0.4,0.3,0.1]) if process=='1' else np.array([7.3,7.0,7.8,7.4,10.1,7.3,5.6,4.2,4.4,3.6,1.7,1.2,0.8,0.0,0.1,0.0])  # using DataThief III
  title = r'Distribution of $m_{CT}$ after all SRA cuts (up to and incl $m_{CT}>150$ GeV), Process '+process
elif plotname == "mbb-3b":
  var = 'mbb'
  f = h5py.File('process'+process+'/ATLAS_8TeV_2bStop_20invfb_SRA_no-mbb-cut.hdf5', 'r')
  dset = f[u'mbb']
  nbins = 15
  binwidth = 40.0
  pltrange = (0,600)
  if normit:
    atlas_data = np.array([0, 0, 3.8, 7.7 , 10.1, 9.5, 9.3, 9.7, 8.9, 6.5, 6.8, 6.2, 5.0, 5.2, 15.7]) if process=='1' else np.array([0.0,0.4,4.9,5.7,8.4,8.4,8.8,9.4,9.1,7.1,5.6,4.3,3.6,3.1,9.5])  # using DataThief III
  else:
    atlas_data = np.array([0, float('nan'), 3.8, 7.7 , 10.1, 9.5, 9.3, 9.7, 8.9, 6.5, 6.8, 6.2, 5.0, 5.2, 15.7]) if process=='1' else np.array([0.0,0.4,4.9,5.7,8.4,8.4,8.8,9.4,9.1,7.1,5.6,4.3,3.6,3.1,9.5])  # using DataThief III
  title = r'Distribution of $m_{bb}$ after all SRA cuts (up to and incl $m_{CT}>150$ GeV)'+'\n'+r'except $m_{bb}>200$ GeV, Process '+process
elif plotname == "met-4a":
  var = 'MET'
  f = h5py.File('process'+process+'/ATLAS_8TeV_2bStop_20invfb_SRB_no-met-cut.hdf5', 'r')
  dset = f[u'met']
  nbins = 17
  binwidth = 20.0
  pltrange = (150,490)
  atlas_data = np.array([23.1,17.4,21.1,20.3,14.5,7.2,11.0,12.3,5.0,3.8,1.1,2.5,2.6,1.4,1.3,0.9,4.3]) if process=='1' else np.array([33.6,42.3,38.7,27.9,22.0,15.9,14.6,15.0,6.1,3.5,2.2,0.8,1.6,0.3,0.0,0.0,1.0])  # using DataThief III
  title = r'Distribution of $E_T^{miss}$ after all SRB cuts except final $E_T^{miss}$ cut, Process '+process
elif plotname == "met-aux2b":
  var = 'MET'
  f = h5py.File('process'+process+'/ATLAS_8TeV_2bStop_20invfb_SRB_all-cuts.hdf5', 'r')
  dset = f[u'met']
  nbins = 10
  binwidth = 20.0
  pltrange = (250,450)
  atlas_data = np.array([7.2,11.1,12.3,5.1,3.9,1.2,2.5,2.6,1.5,6.7]) if process=='1' else np.array([15.9,14.6,15.1,6.2,3.5,2.2,0.9,1.6,0.3,1.1])  # using DataThief III
  title = r'Distribution of $E_T^{miss}$ after all SRB cuts, Process '+process
elif plotname == "ht3-4b":
  var = 'HT3'
  f = h5py.File('process'+process+'/ATLAS_8TeV_2bStop_20invfb_SRB_no-ht3-cut.hdf5', 'r')
  dset = f[u'ht3']
  nbins = 20
  binwidth = 10.0
  pltrange = (0,200)
  atlas_data = np.array([20.3,0.0,21.2,4.8,7.8,6.9,7.2,5.2,8.1,1.4,0.8,4.1,1.4,0.1,0.7,1.3,1.7,0.0,0.6,3.5]) if process=='1' else np.array([21.3,0.0,25.2,5.8,9.0,7.7,7.0,2.8,6.2,2.9,2.2,4.9,3.2,0.9,1.9,0.0,2.2,0.1,0.0,3.5])  # using DataThief III
  title = r'Distribution of $H_{T3}$ after all SRB cuts except final $H_{T3}$ cut, Process '+process
else:
  print('You need to specify which plot to replicate!\nYour options are: mct-3a, mbb-3b, met-4a, ht3-4b, mct-aux2a, met-aux2b\nYou also need to specify process: "1" or "2"')
  exit()


bin_centers = (np.linspace(*pltrange, num=nbins+1) + binwidth/2)[:-1]

vals = dset[0,:]
factor = 1.0/binwidth
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
plt.ylabel("Events / {} GeV".format(int(binwidth)))
plt.legend()
if normit:
  plt.title('(NORM) ' + title)
  plt.savefig(plotname+'-pr'+str(process)+'-norm'+'.png',dpi=250)
else:
  plt.title(title)
  plt.savefig(plotname+'-pr'+str(process)+'.png',dpi=250)

plt.show()


