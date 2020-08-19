import matplotlib.pyplot as plt
import h5py
import numpy as np
import sys


plotname = sys.argv[1]
if plotname == "mct-3a":
  var = 'mCT'
  f = h5py.File('ATLAS_8TeV_2bStop_20invfb_SRA_no-mct-cut.hdf5', 'r')
  dset = f[u'mct']
  nbins = 20
  binwidth = 25.0
  pltrange = (0,500)
#  atlas_data = np.array([0, 0.7, 2.0, 2.1, 3.4, 5.4, 7.1, 8.5, 7.0, 8.7, 7.8, 8.1, 7.2, 7.3, 7.2, 4.6, 3.9, 3.8, 2.1, 1.0])  # from my own observations
  atlas_data = np.array([0.1, 0.3, 1.9, 2.0, 3.3, 5.3, 7.2, 8.4, 6.9, 8.5, 7.6, 8.1, 6.8, 7.2, 7.0, 4.4, 3.6, 3.5, 1.9, 0.8])  # using DataThief III
  title = r'Distribution of $m_{CT}$ in SRA with all SRA cuts except $m_{CT}$ cuts'
elif plotname == "mct-aux2a":
  var = 'mCT'
  f = h5py.File('ATLAS_8TeV_2bStop_20invfb_SRA_no-mct-cut.hdf5', 'r')
  dset = f[u'mct']
  nbins = 16
  binwidth = 25.0
  pltrange = (150,550)
  atlas_data = np.array([7.2,8.5,7.0,8.7,7.7,8.1,6.9,7.2,7.1,4.6,3.8,3.6,2.0,0.4,0.3,0.1])  # using DataThief III
  title = r'Distribution of $m_{CT}$ after all SRA cuts up to and incl $m_{CT}>150$ GeV'
elif plotname == "mbb-3b":
  var = 'mbb'
  f = h5py.File('ATLAS_8TeV_2bStop_20invfb_SRA_no-mbb-cut.hdf5', 'r')
  dset = f[u'mbb']
  nbins = 15
  binwidth = 40.0
  pltrange = (0,600)
  atlas_data = np.array([0, float('nan'), 3.8, 7.7 , 10.1, 9.5, 9.3, 9.7, 8.9, 6.5, 6.8, 6.2, 5.0, 5.2, 15.7])  # using DataThief III
  title = r'Distribution of $m_{bb}$ in SRA with all SRA cuts incl $m_{ct}>150$ GeV'
elif plotname == "met-4a":
  var = 'MET'
  f = h5py.File('ATLAS_8TeV_2bStop_20invfb_SRB_no-met-cut.hdf5', 'r')
  dset = f[u'met']
  nbins = 17
  binwidth = 20.0
  pltrange = (150,490)
  atlas_data = np.array([23.1,17.4,21.1,20.3,14.5,7.2,11.0,12.3,5.0,3.8,1.1,2.5,2.6,1.4,1.3,0.9,4.3])  # using DataThief III
  title = r'Distribution of $E_T^{miss}$ in SRB with all SRB cuts except final $E_T^{miss}$ cut'
elif plotname == "met-aux2b":
  var = 'MET'
  f = h5py.File('ATLAS_8TeV_2bStop_20invfb_SRB_all-cuts.hdf5', 'r')
  dset = f[u'met']
  nbins = 10
  binwidth = 20.0
  pltrange = (250,450)
  atlas_data = np.array([7.2,11.1,12.3,5.1,3.9,1.2,2.5,2.6,1.5,6.7])  # using DataThief III
  title = r'Distribution of $E_T^{miss}$ after all SRB cuts'
elif plotname == "ht3-4b":
  var = 'HT3'
  f = h5py.File('ATLAS_8TeV_2bStop_20invfb_SRB_no-ht3-cut.hdf5', 'r')
  dset = f[u'ht3']
  nbins = 20
  binwidth = 10.0
  pltrange = (0,200)
  atlas_data = np.array([20.3,0.0,21.2,4.8,7.8,6.9,7.2,5.2,8.1,1.4,0.8,4.1,1.4,0.1,0.7,1.3,1.7,0.0,0.6,3.5])  # using DataThief III
  title = r'Distribution of $H_{T3}$ in SRB with all SRB cuts except final $H_{T3}$ cut'
else:
  print('You need to specify which plot to replicate!\nYour options are: mct-3a, mbb-3b, met-4a, ht3-4b, mct-aux2a, met-aux2b')
  exit()


bin_centers_init = np.linspace(*pltrange, num=nbins+1) + binwidth/2
bin_centers = bin_centers_init[:-1]
bin_centers_plot = np.concatenate(([pltrange[0]-binwidth/2],bin_centers,[pltrange[1]+binwidth/2]), axis=0)
atlas_data_plot = np.concatenate(([0],atlas_data,[0]), axis=0)

atlas_uncert = 0.4  # width of line on plot is 0.7
#atlas_uncert = 0.07  # cursor jumps by a minimum of 0.07 on the y axis wait never mind I just zoomed in more
atlas_uncert_plot = np.concatenate(([0], atlas_uncert*np.ones_like(bin_centers), [0]), axis=0)
print(atlas_uncert_plot.shape)
print(atlas_data_plot.shape)
print(bin_centers_plot.shape)

vals = dset[0,:]
factor = 1.0/binwidth
plt.hist(np.clip(vals,*pltrange), bins=nbins,range=pltrange, weights=factor*np.ones_like(vals), histtype='step', color='blue', label='CBS')
plt.errorbar(bin_centers_plot, atlas_data_plot, yerr=atlas_uncert_plot, marker='', ls='-', drawstyle='steps-mid', color='black', label='ATLAS')
plt.xlabel("{} [GeV]".format(var))
plt.ylabel("Events / {} GeV".format(int(binwidth)))
plt.legend()
plt.title(title)

plt.savefig(plotname+'.png',dpi=250)

plt.show()


