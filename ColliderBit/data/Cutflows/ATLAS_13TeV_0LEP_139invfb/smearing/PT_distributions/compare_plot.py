 #!/usr/bin/env python
import os
import sys
from pylab import *
import pandas
import random
import matplotlib.colors as mcolors
cmap = plt.get_cmap("tab10")


#name = sys.argv[1]

#normalize = len(sys.argv)>2


print len(sys.argv)
if len(sys.argv)<2:
    print sys.argv
    ID = 0
else:
    ID = int(sys.argv[1])

num_bins = 100
normalize = False
xmin=0
xmax=1500
if ID == 0:
    name = r"$E_T^{miss}$"
    xcut = 300
elif ID == 1:
    name = r"$E_T^{miss}/\sqrt{H_T}$"
    xmax=50
    xcut = 16
elif ID == 2:
    name = r"$M_{eff}$"
    xmax=3000
    xcut = 800
elif ID == 3:
    name = r"$\sqrt{H_T}$"
    xmax=80
    xcut=0
elif ID == 4:
    name = r"$P_T(j_1)$"
    xmax=1000
    xcut = 600
elif ID == 5:
    name = r"$P_T(j_2)$"
    xmax=600
    xcut = 250

CBS_0j_139_smeared = np.loadtxt("CBS_0j_139_smeared.txt")
CBS_0j_139_nosmearing = np.loadtxt("CBS_0j_139_nosmearing.txt")
Delphes_0j_139_smeared = np.loadtxt("Delphes_0j_139_smeared.txt")
Delphes_0j_139_nosmearing = np.loadtxt("Delphes_0j_139_nosmearing.txt")

C_smeared = CBS_0j_139_smeared[:,ID]
C_nosmearing = CBS_0j_139_nosmearing[:,ID]
D_smeared = Delphes_0j_139_smeared[:,ID]
D_nosmearing = Delphes_0j_139_nosmearing[:,ID]


fig = plt.figure(figsize=(5,5))
plt.hist(C_nosmearing, num_bins, density=normalize, range=[xmin, xmax], histtype=u'step', label='No smearing', alpha=0.9,linewidth=2)
plt.hist(C_smeared, num_bins, density=normalize, range=[xmin, xmax], histtype=u'step', label='CBS smeared', alpha=0.9,linewidth=2)
plt.hist(D_smeared, num_bins, density=normalize, range=[xmin, xmax], histtype=u'step', label='Delphes smeared', 
alpha=0.9,linewidth=2)

plt.axvline(x=xcut,color='gray', linestyle='--')

#plt.hist(D_nosmearing, num_bins, density=normalize, histtype=u'step',  alpha=0.9)


plt.legend(loc=1)

plt.ylabel("Event yields")

plt.xlabel(name.replace(r"/",r"_"))
plt.xlim(xmin, xmax)

plt.tight_layout()

plt.savefig(name[1:-1]+'.png',dpi=250)


plt.show()
