 #!/usr/bin/env python
import os
import sys
from pylab import *
import pandas
import random
import matplotlib.colors as mcolors
cmap = plt.get_cmap("tab10")


# name of file passed in
name = sys.argv[1]
# whether or not to normalize
normalize = len(sys.argv)>2
# whether this is before or after my edits
before = name[-10:-4] == 'BEFORE'

print("\n~~~~~~~~~~~~~~")
print("The program identifies which process you're doing by the .txt file you pass in (pr1/pr2/pr3)!")
print("We do NOT have pr4 support at this time :(")
print("BTW the file path is hard-coded in so just the name is fine.")
print("You are doing: " + name[:-4])
print("~~~~~~~~~~~~~~\n")


# get data as 2d array of strings, with same rows & columns as .txt file
data = np.genfromtxt("data_2LEPEW/" + name,dtype="str")
name = name[:-4]  # all except .txt
process = int(name[-8]) if before else int(name[-1])
bad = data == 'no_data'
data[bad] = np.nan

evtcount = 50000
evtcount_cbs = 250000
num_SRs = 2 if (process == 1 or process == 3) else 3
if process == 1:
  xsect = 10.3  #fb
  xsect_uncert = 0.0227*xsect
  SRmumu = 6
elif process == 2:
  xsect = 13.60409
  xsect_uncert = 0.1744570*xsect
  SRmumu = 6
  SRemu = 12
elif process == 3:
  xsect = 70.69 
  xsect_uncert = 0.05439*xsect
  SRmumu = 9
#elif process == '4':
#  xsect = 85.5847
#  xsect_uncert = 0.149611*xsect
#  SRmumu = 69
#  SRemu = 420
#  scaling = 1.0

x = range(len(data))
ma_tuned = data.shape[1] == 5

atlas = data[:,0].astype(np.float)
atlas_ee = atlas[:SRmumu]
atlas_mumu = atlas[SRmumu:SRemu] if num_SRs == 3 else atlas[SRmumu:]
atlas_emu = atlas[SRemu:] if num_SRs == 3 else atlas[:]
cbs = data[:,3].astype(np.float)
cm = data[:,1].astype(np.float)
ma = data[:,2].astype(np.float)
#if ma_tuned:
#  matune = data[:,4].astype(np.float)

if normalize:
    normalizer = atlas
    zero = 1
else:
    normalizer = 1
    zero = 0    

factor = xsect*20.3/evtcount  # same xsect for all SRs (ee/mumu/emu because same process... this doesn't apply to pr4!)
factor_cbs = xsect*20.3/242500  # for some reason this was the initial evt count for CBS for LEPEW analysis
if normalize:
  cbs_err = np.sqrt(cbs*factor_cbs)/normalizer
  cm_err = np.sqrt(cm*factor)/normalizer
  ma_err = np.sqrt(ma*factor)/normalizer
else:
  cbs_err = np.sqrt(cbs*factor_cbs)
  cm_err = np.sqrt(cm*factor)
  ma_err = np.sqrt(ma*factor)

#  if ma_tuned:
#    matune_err = sqrt(matune/factor)/norm

cbs = cbs/normalizer -zero
cm = cm/normalizer -zero
ma = ma/normalizer -zero
atlas = atlas/normalizer -zero
#print("CBS: ", cbs)
#print("CM2: ", cm)
#print("MA5: ", ma)

cbs_label = 'CBS'
cm_label = 'CM2'
ma_label = 'MA5'
atlas_label = 'ATLAS'

fig = plt.figure(figsize=(10,6))

plt.errorbar(x, cm, yerr=cm_err, color=cmap(1), fmt='x', label=cm_label)
plt.errorbar(x, ma, yerr=ma_err, color=cmap(2), fmt='D', label=ma_label)
plt.errorbar(x, cbs, yerr=cbs_err, color=cmap(0), fmt='o', label=cbs_label)
plt.step(x[0:SRmumu], atlas[0:SRmumu], where='mid', label=atlas_label, color=cmap(3))
if num_SRs == 2:
  plt.step(x[SRmumu:], atlas[SRmumu:], where='mid', color='green')
elif num_SRs == 3:
  plt.step(x[SRmumu:SRemu], atlas[SRmumu:SRemu], where='mid', color='green')
  plt.step(x[SRemu:], atlas[SRemu:], where='mid', color='blue')

#if ma_tuned:
#  plt.errorbar(x, matune, yerr=matune_err, color=cmap(2.2), fmt='*', label='MA5tune')

#plt.step(x, atlas, where='mid', label='ATLAS', color=cmap(3))  # this was commented out when I got here

if normalize:
  plt.legend(loc=1)
else:
  plt.legend(loc=1)


if process == 1:
  sr_name = [r"Two signal $e^+e^-$",
  r"Jet veto",
  r"Z veto",
  r"SR-$m_{T2}^{90}$",
  r"SR-$m_{T2}^{120}$",
  r"SR-$m_{T2}^{150}$",
  r"Two signal $\mu^+\mu^-$",
  r"Jet veto",
  r"Z veto",
  r"SR-$m_{T2}^{90}$",
  r"SR-$m_{T2}^{120}$",
  r"SR-$m_{T2}^{150}$"]
elif process == 2:
  sr_name = [r"Two signal $e^+e^-$",
  r"Jet veto",
  r"Z veto",
  r"SR-$m_{T2}^{90}$",
  r"SR-$m_{T2}^{120}$",
  r"SR-$m_{T2}^{150}$",
  r"Two signal $\mu^+\mu^-$",
  r"Jet veto",
  r"Z veto",
  r"SR-$m_{T2}^{90}$",
  r"SR-$m_{T2}^{120}$",
  r"SR-$m_{T2}^{150}$",
  r"Two signal $\mu^\pm e^\pm$",
  r"Jet veto",
  r"Z veto",
  r"SR-$m_{T2}^{90}$",
  r"SR-$m_{T2}^{120}$",
  r"SR-$m_{T2}^{150}$"]
elif process == 3:
  sr_name = [r"Two signal $e^+e^-$",
  r"$>1$ light jets",
  r"No b- and forward jets",
  r"Z window",
  r"$p_{T,ll}>80$ GeV",
  r"$E_T^{miss,rel}>80$ GeV",
  r"$0.3 < \Delta R_{ll} < 1.5$",
  r"$50 < m_{jj} < 100$ GeV",
  r"Jet $p_T > 45$ GeV",
  r"Two signal $\mu^+\mu^-$",
  r"$>1$ light jets",
  r"No b- and forward jets",
  r"Z window",
  r"$p_{T,ll}>80$ GeV",
  r"$E_T^{miss,rel}>80$ GeV",
  r"$0.3 < \Delta R_{ll} < 1.5$",
  r"$50 < m_{jj} < 100$ GeV",
  r"Jet $p_T > 45$ GeV"]

plt.xticks(x,sr_name,rotation=90)

if process == 1:
  if normalize:
    plt.ylabel("Relative difference to ATLAS event yields")
  #  plt.yticks([-0.3,-0.2,-0.1,0,0.1,0.2,0.3],['-30%','-20%','-10%',0,'10%','20%','30%'],rotation=90)
    plt.yticks([-1.0,-0.8,-0.6,-0.4,-0.2,0],['-100%','-80%','-60%','-40%','-20%',0],rotation=90)
    plt.ylim(-1.25,0.05)    
  else:
    plt.ylabel("Event yields")
elif process == 2:
  if normalize:
    plt.ylabel("Relative difference to ATLAS event yields")
    plt.yticks([-0.1,0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8],['-10%',0,'10%','20%','30%','40%','50%','60%','70%','80%'],rotation=90)
  #  plt.yticks([-0.3,-0.2,-0.1,0,0.1,0.2,0.3],['-30%','-20%','-10%',0,'10%','20%','30%'],rotation=90)
    plt.ylim(-0.15,0.95)
  else:
    plt.ylabel("Event yields")
elif process == 3:
  if normalize:
    plt.ylabel("Relative difference to ATLAS event yields")
  #  plt.yticks([-0.8,-0.7,-0.6,-0.5,-0.4,-0.3,-0.2,-0.1,0,0.1],['-80%','-70%','-60%','-50%','-40%','-30%','-20%','-10%',0,'10%','20%','30%'],rotation=90)
    plt.yticks([-0.8,-0.6,-0.4,-0.2,0,0.2,0.4],['-80%','-60%','-40%','-20%',0,'20%','40%'],rotation=90)
  #  plt.yticks([-0.7,-0.6,-0.5,-0.4,-0.3,-0.2,-0.1,0,0.1,0.2,0.3],['-70%','-60%','-50%','-40%','-30%','-20%','-10%',0,'10%','20%','30%'],rotation=90)
    plt.ylim(-0.75,0.55)
  else:
    plt.ylabel("Event yields")    


plt.tight_layout()

save_name = "ATLAS-" + name

if normalize:
    plt.savefig(name+'.png',dpi=250)
else:
    plt.savefig('norm-'+name+'.png',dpi=250)

plt.title("Cutflow comparison")
plt.show()
