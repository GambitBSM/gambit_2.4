 #!/usr/bin/env python
import os
import sys
from pylab import *
import pandas
import random
import matplotlib.colors as mcolors
cmap = plt.get_cmap("tab10")


name = sys.argv[1]

normalize = len(sys.argv)>2


data = np.genfromtxt(name,dtype="str")
name = name[:-4]

x = range(len(data))
ma_tuned = data.shape[1] == 5

atlas = data[:,0].astype(np.float)

if normalize:
    norm = atlas
    zero = 1
else:
    norm = 1
    zero = 0
    
cbs = data[:,1].astype(np.float)
cm = data[:,2].astype(np.float)
ma = data[:,3].astype(np.float)
if ma_tuned:
  matune = data[:,4].astype(np.float)
if name == 'tab_16':
  factor = 90299./1763.
  cbs_err = sqrt(cbs/factor)/norm
  cm_err = sqrt(cm/factor)/norm
  ma_err = sqrt(ma/factor)/norm
  if ma_tuned:
   matune_err = sqrt(matune/factor)/norm
else:
  cbs_err = 0
  cm_err = 0
  ma_err = 0
  
cbs = cbs/norm -zero
cm = cm/norm -zero
ma = ma/norm -zero
if ma_tuned:
  matune = matune/norm -zero
atlas = atlas/norm -zero


fig = plt.figure(figsize=(10,6))

plt.errorbar(x, cbs, yerr=cbs_err, color=cmap(0), fmt='o', label='CBS')
plt.errorbar(x, cm, yerr=cm_err, color=cmap(1), fmt='x', label='CM2')
plt.errorbar(x, ma, yerr=ma_err, color=cmap(2), fmt='D', label='MA5')
if ma_tuned:
  plt.errorbar(x, matune, yerr=matune_err, color=cmap(2.2), fmt='*', label='MA5tune')
#plt.step(x, atlas, where='mid', label='ATLAS', color=cmap(3))

if name == 'tab_16':
  SR1 = 8
  SR2 = 13
elif name == 'tab_17':
  SR1 = 9
  SR2 = 17
else:
  SR1 = 7
  SR3 = 15
  SR2 = 23

plt.step(x[0:SR1], atlas[0:SR1], where='mid', label='ATLAS', color=cmap(3))
if name == 'tab_18':
  plt.step(x[SR1:SR3], atlas[SR1:SR3], where='mid', color=cmap(3))
  plt.step(x[SR3:SR2], atlas[SR3:SR2], where='mid', color=cmap(3))
else:
  plt.step(x[SR1:SR2], atlas[SR1:SR2], where='mid', color=cmap(3))
plt.step(x[SR2:], atlas[SR2:], where='mid', color=cmap(3))

if normalize:
  plt.legend(loc=3)
else:
  plt.legend(loc=1)

if name == 'tab_16':
  sr_name = ["Pre-selection, etc",
  r"$N_{\rm jets}>=2$",
  r"$\Delta \phi({\rm jet}_{1,2,(3)},\vec{P}_T^{miss})_{\rm min}>0.8$",
  r"$\Delta \phi({\rm jet}_{i>3},\vec{P}_T^{miss})_{\rm min}>0.4$",
  r"$p_T({\rm jet}_2)>250$ GeV",
  r"$|\eta(j_{1,2})|<2.0$",
  r"$E_T^{\rm miss}/\sqrt{H_T}>16$ GeV$^{1/2}$",
  r"$m_{\rm eff}>1600$ GeV",
  r"$\Delta \phi({\rm jet}_{1,2,(3)},\vec{P}_T^{miss})_{\rm min}>0.4$",
  r"$\Delta \phi({\rm jet}_{i>3},\vec{P}_T^{miss})_{\rm min}>0.2$",
  r"$p_T({\rm jet}_1)>600$ GeV",
  r"$E_T^{\rm miss}/\sqrt{H_T}>16$ GeV$^{1/2}$",
  r"$m_{\rm eff}>2200$ GeV",
  r"$\Delta \phi({\rm jet}_{1,2,(3)},\vec{P}_T^{miss})_{\rm min}>0.8$",
  r"$\Delta \phi({\rm jet}_{i>3},\vec{P}_T^{miss})_{\rm min}>0.4$",
  r"$p_T({\rm jet}_2)>250$ GeV",
  r"$|\eta(j_{1,2})|<1.2$",
  r"$E_T^{\rm miss}/\sqrt{H_T}>16$ GeV$^{1/2}$",
  r"$m_{\rm eff}>2800$ GeV"]
elif name == 'tab_17':
  sr_name = ["Pre-selection, etc",
  r"$N_{\rm jets}>=4$",
  r"$\Delta \phi({\rm jet}_{1,2,(3)},\vec{P}_T^{miss})_{\rm min}>0.4$",
  r"$\Delta \phi({\rm jet}_{i>3},\vec{P}_T^{miss})_{\rm min}>0.2$",
  r"$p_T({\rm jet}_4)>100$ GeV",
  r"$|\eta(j_{1,2,3,4})|<2.0$",
  r"Aplanarity$>0.04$",
  r"$E_T^{\rm miss}/\sqrt{H_T}>16$ GeV$^{1/2}$",
  r"$m_{\rm eff}>1000$ GeV",
  r"$N_{\rm jets}>=4$",
  r"$\Delta \phi({\rm jet}_{1,2,(3)},\vec{P}_T^{miss})_{\rm min}>0.4$",
  r"$\Delta \phi({\rm jet}_{i>3},\vec{P}_T^{miss})_{\rm min}>0.2$",
  r"$p_T({\rm jet}_4)>100$ GeV",
  r"$|\eta(j_{1,2,3,4})|<2.0$",
  r"Aplanarity$>0.04$",
  r"$E_T^{\rm miss}/\sqrt{H_T}>16$ GeV$^{1/2}$",
  r"$m_{\rm eff}>2200$ GeV",
  r"$N_{\rm jets}>=4$",
  r"$\Delta \phi({\rm jet}_{1,2,(3)},\vec{P}_T^{miss})_{\rm min}>0.4$",
  r"$\Delta \phi({\rm jet}_{i>3},\vec{P}_T^{miss})_{\rm min}>0.2$",
  r"$p_T({\rm jet}_4)>100$ GeV",
  r"$|\eta(j_{1,2,3,4})|<2.0$",
  r"Aplanarity$>0.04$",
  r"$E_T^{\rm miss}/\sqrt{H_T}>10$ GeV$^{1/2}$",
  r"$m_{\rm eff}>3400$ GeV"]
else:
  sr_name = ["Pre-selection, etc",
  r"$N_{\rm jets}>=5$",
  r"$\Delta \phi({\rm jet}_{1,2,(3)},\vec{P}_T^{miss})_{\rm min}>0.4$",
  r"$\Delta \phi({\rm jet}_{i>3},\vec{P}_T^{miss})_{\rm min}>0.2$",
  r"$p_T({\rm jet}_1)>600$ GeV",
  r"$E_T^{\rm miss}/\sqrt{H_T}>16$ GeV$^{1/2}$",
  r"$m_{\rm eff}>1600$ GeV",
  r"$N_{\rm jets}>=6$",
  r"$\Delta \phi({\rm jet}_{1,2,(3)},\vec{P}_T^{miss})_{\rm min}>0.4$",
  r"$\Delta \phi({\rm jet}_{i>3},\vec{P}_T^{miss})_{\rm min}>0.2$",
  r"$p_T({\rm jet}_6)>75$ GeV",
  r"$|\eta(j_{1,2,3,4,5,6})|<2.0$",
  r"Aplanarity$>0.08$",
  r"$E_T^{\rm miss}/\sqrt{H_T}>16$ GeV$^{1/2}$",
  r"$m_{\rm eff}>1000$ GeV",
  r"$N_{\rm jets}>=6$",
  r"$\Delta \phi({\rm jet}_{1,2,(3)},\vec{P}_T^{miss})_{\rm min}>0.4$",
  r"$\Delta \phi({\rm jet}_{i>3},\vec{P}_T^{miss})_{\rm min}>0.2$",
  r"$p_T({\rm jet}_6)>75$ GeV",
  r"$|\eta(j_{1,2,3,4,5,6})|<2.0$",
  r"Aplanarity$>0.08$",
  r"$E_T^{\rm miss}/\sqrt{H_T}>16$ GeV$^{1/2}$",
  r"$m_{\rm eff}>2200$ GeV",
  r"$N_{\rm jets}>=6$",
  r"$\Delta \phi({\rm jet}_{1,2,(3)},\vec{P}_T^{miss})_{\rm min}>0.4$",
  r"$\Delta \phi({\rm jet}_{i>3},\vec{P}_T^{miss})_{\rm min}>0.2$",
  r"$p_T({\rm jet}_6)>75$ GeV",
  r"$|\eta(j_{1,2,3,4,5,6})|<2.0$",
  r"Aplanarity$>0.08$",
  r"$E_T^{\rm miss}/\sqrt{H_T}>10$ GeV$^{1/2}$",
  r"$m_{\rm eff}>3400$ GeV"]



plt.xticks(x,sr_name,rotation=90)

if normalize:
  plt.ylabel("Relative difference to ATLAS event yields")
  plt.yticks([-0.3,-0.2,-0.1,0,0.1,0.2,0.3],['-30%','-20%','-10%',0,'10%','20%','30%'],rotation=90)
  plt.ylim(-0.35,0.35)
  if name == 'tab_18':
    plt.ylim(-0.35,0.55)
    
else:
    plt.ylabel("Event yields")

plt.tight_layout()


if normalize:
    plt.savefig('compare_norm_'+name+'.png',dpi=250)
else:
    plt.savefig('compare_'+name+'.png',dpi=250)

plt.title("Kelton's normalized cutflow comparison")
plt.show()
