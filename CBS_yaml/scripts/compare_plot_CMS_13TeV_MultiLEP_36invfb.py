 #!/usr/bin/env python
import os
import sys
from pylab import *
import pandas
import random
import matplotlib.colors as mcolors
cmap = plt.get_cmap("tab10")

print("\n~~~~~~~~~~~~~~")
print("Pass in 1 or 2 args: (1) the file containing data to be plotted, (2) any 2nd argument if you wanna normalize wrt CMS")
print("The script will figure out which process you're doing from the name of the .txt file.")
print("The file path (data_MultiLEP/.) is hard-coded in so just enter the name.")
print("~~~~~~~~~~~~~~\n")

# name of file passed in
name = sys.argv[1]
# whether or not to normalize
normalize = len(sys.argv)>2
# whether this is before or after my edits
before = name[-10:-4] == 'BEFORE'
# find out the process, and for pr4, whether it's ee, mumu or emu
if 'pr1' in name:
  process = 1
if 'pr2' in name:
  process = 2
if 'pr3' in name:
  process = 3
if 'pr4' in name:
  process = 4

print("You are doing: " + name[:-4])
print("Process: " + str(process))

# get data as 2d array of strings, with same rows & columns as .txt file
data = np.genfromtxt("data_MultiLEP/" + name,dtype="str", skip_header=1)
name = name[:-4]  # all except .txt
bad = data == 'no_data'
data[bad] = np.nan

evtcount = 50000
if process == 1:
  xsect = 1800  #fb
  xsect_uncert = 0.0*xsect
elif process == 2:
  xsect = 780
  xsect_uncert = 0.0*xsect
elif process == 3:
  xsect = 46
  xsect_uncert = 0.0*xsect
elif process == 4:
  xsect = 16800
  xsect_uncert = 0.0*xsect_1

x = range(len(data))
ma_tuned = data.shape[1] == 5

atlas = data[:,0].astype(np.float)
cbs = data[:,3].astype(np.float)
cm = data[:,1].astype(np.float)
ma = data[:,2].astype(np.float)

if normalize:
  normalizer = atlas
  zero = 1
else:
  normalizer = 1
  zero = 0    

factor = xsect*35.9/evtcount 
cbs_err = np.sqrt(cbs*factor)/normalizer
cm_err = np.sqrt(cm*factor)/normalizer
ma_err = np.sqrt(ma*factor)/normalizer
if ma_tuned:
  matune_err = sqrt(matune*factor)/normalizer

atlas = atlas/normalizer -zero
cbs = cbs/normalizer -zero
cm = cm/normalizer -zero
ma = ma/normalizer -zero

#print("CBS: ", cbs)  # for debugging
#print("CM2: ", cm)
#print("MA5: ", ma)


# ~~~~~~~~~~ plotting ~~~~~~~~~~

fig = plt.figure(figsize=(10,6))

cbs_label = 'CBS'
cm_label = 'CM2'
ma_label = 'MA5'
atlas_label = 'CMS'

plt.errorbar(x, cm, yerr=cm_err, color=cmap(1), fmt='x', label=cm_label)
#plt.errorbar(x, ma, yerr=ma_err, color=cmap(2), fmt='D', label=ma_label)  # commented out until we get output
plt.errorbar(x, cbs, yerr=cbs_err, color=cmap(0), fmt='o', label=cbs_label)
plt.step(x, atlas, where='mid', label=atlas_label, color=cmap(3))
if ma_tuned:
  plt.errorbar(x, matune, yerr=matune_err, color=cmap(2.2), fmt='*', label='MA5tune')

if normalize:
  plt.legend(loc=4)
else:
  plt.legend(loc=1)

if process == 1:
  sr_name = [r"3 tight leptons",
  r"4th lepton veto",
  r"Low-mass & conversions veto",
  r"b-jet veto",
  r"$E_T^{miss}>50$ GeV",
  r"$M_T>100$ GeV",
  r"$M_{ll}>75$ GeV"]
elif process == 2:
  sr_name = [r"3 tight leptons",
  r"4th lepton veto",
  r"Low-mass & conversions veto",
  r"b-jet veto",
  r"$E_T^{miss}>50$ GeV",
  r"$M_T2<100$ GeV",
  r"$M_{ll}<75$ GeV"]
elif process == 3:
  sr_name = [r"2 tight $e$ or $\mu$",
  r"Same sign",
  r"3rd lepton veto",
  r"Low-mass veto",
  r"b-jet veto",
  r"$E_T^{miss}>60$ GeV",
  r"0 or 1 ISR jet",
  r"$M_T<100$ GeV",
  r"$p_T^{ll}>100$ GeV"]
elif process == 4:
  sr_name = [r"4 tight leptons",
  r"Low-mass veto",
  r"b-jet veto",
  r"$E_T^{miss}>100$ GeV"]

plt.xticks(x,sr_name,rotation=90)

if process == 1:
  if normalize:
    plt.ylabel("Relative difference to CMS event yields")
  #  plt.yticks([-0.3,-0.2,-0.1,0,0.1,0.2,0.3],['-30%','-20%','-10%',0,'10%','20%','30%'],rotation=90)
    plt.yticks([-0.8,-0.6,-0.4,-0.2,0,0.2,0.4,0.6,0.8],['-80%','-60%','-40%','-20%',0,'20%','40%','60%','80%'],rotation=90)
    plt.ylim(-1.05,0.95)
  else:
    plt.ylabel("Event yields")
elif process == 2:
  if normalize:
    plt.ylabel("Relative difference to CMS event yields")
    plt.yticks([0,0.8,1.6,2.4,3.2,4.0],[0,'80%','160%','240%','320%','400%'],rotation=90)
  #  plt.yticks([-0.3,-0.2,-0.1,0,0.1,0.2,0.3],['-30%','-20%','-10%',0,'10%','20%','30%'],rotation=90)
    plt.ylim(-0.05,4.25)
  else:
    plt.ylabel("Event yields")
elif process == 3:
  if normalize:
    plt.ylabel("Relative difference to CMS event yields")
    plt.yticks([-0.4,-0.2,0,0.2,0.4],['-40%','-20%',0,'20%','40%'],rotation=90)
  #  plt.yticks([-0.7,-0.6,-0.5,-0.4,-0.3,-0.2,-0.1,0,0.1,0.2,0.3],['-70%','-60%','-50%','-40%','-30%','-20%','-10%',0,'10%','20%','30%'],rotation=90)
    plt.ylim(-0.55,0.35)
    plt.legend(loc=3)
  else:
    plt.ylabel("Event yields")
elif process == 4:
  if normalize:
    plt.ylabel("Relative difference to CMS event yields")
    plt.yticks([-0.8,-0.6,-0.4,-0.2,0,0.2,0.4],['-80%','-60%','-40%','-20%',0,'20%','40%'],rotation=90)
  #  plt.yticks([-0.7,-0.6,-0.5,-0.4,-0.3,-0.2,-0.1,0,0.1,0.2,0.3],['-70%','-60%','-50%','-40%','-30%','-20%','-10%',0,'10%','20%','30%'],rotation=90)
    plt.ylim(-0.75,0.55)
  else:
    plt.ylabel("Event yields")

plt.tight_layout()

#save_name = "CMS-" + name
save_name = name

if before:
  if normalize:
      plt.savefig('norm-'+save_name[:-7]+'-before.png',dpi=250)
  else:
      plt.savefig(save_name[:-7]+'-before.png',dpi=250)
else:
  if normalize:
      plt.savefig('norm-'+save_name+'.png',dpi=250)
  else:
      plt.savefig(save_name+'.png',dpi=250)


plt.title("Cutflow comparison")
plt.show()
