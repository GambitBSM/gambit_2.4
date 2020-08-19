 #!/usr/bin/env python
import os
import sys
from pylab import *
import pandas
import random
import matplotlib.colors as mcolors
cmap = plt.get_cmap("tab10")

# name of file passed in
name_a = sys.argv[1]
name_b = sys.argv[2]
name_c = sys.argv[3]
# whether or not to normalize
normalize = len(sys.argv)>4


# get data as 2d array of strings, with same rows & columns as .txt file
data_a = np.genfromtxt(name_a,dtype="str")
data_b = np.genfromtxt(name_b,dtype="str")
data_c = np.genfromtxt(name_c,dtype="str")
name_a = name_a[:-4]  # all except .txt
name_b = name_b[:-4]
# print(data_a)
# print(data_b)
data = np.concatenate((data_a, data_b, data_c), axis=0)
bad = data == 'no_data'
data[bad] = np.nan
print(data)


x = range(len(data))
ma_tuned = data.shape[1] == 5

mid_1 = 19  # row where the first file ends
mid_2 = 34  # row where the 2nd file ends

atlas = data[:,0].astype(np.float)
atlas_a = atlas[:mid_1]
atlas_b = atlas[mid_1:mid_2]
atlas_c = atlas[mid_2:]
print(atlas_a)
print(atlas_b)
print(atlas_c)

if normalize:
    norm_a = atlas_a
    norm_b = atlas_b
    norm_c = atlas_c
    zero_a = 1
    zero_b = 1
    zero_c = 1
else:
    norm_a = 1
    norm_b = 1
    norm_c = 1
    zero_a = 0    
    zero_b = 0    
    zero_c = 0    

cbs = data[:,3].astype(np.float)
cm = data[:,1].astype(np.float)
ma = data[:,2].astype(np.float)
#if ma_tuned:
#  matune = data[:,4].astype(np.float)
#if name == 'tab_16':  # I don't know how this works
#  factor = 90299./1738.
#  cbs_err = sqrt(cbs/factor)/norm
#  cm_err = sqrt(cm/factor)/norm
##  ma_err = sqrt(ma/factor)/norm
#  if ma_tuned:
#    matune_err = sqrt(matune/factor)/norm
#else:
#  cbs_err = 0
#  cm_err = 0
#  ma_err = 0

ma_err = 0 
cbs_err = 0
cm_err = 0

#print(cbs)
#print(shape(cbs))
#print(len(cbs[:10]))
#print(len(cbs[10:]))
#print(len(norm_a))
#print(len(norm_b))

#scaling = 1.0/10  # because otherwise you can't see any detail on SRA
scaling = 1.0

#if normalize != True:
#  cbs = np.concatenate((cbs[:mid]/norm_a -zero_a, cbs[mid:]*scaling/norm_b -zero_b),axis=0)
#  cm = np.concatenate((cm[:mid]/norm_a -zero_a, cm[mid:]*scaling/norm_b -zero_b),axis=0)
#  ma = np.concatenate((ma[:mid]/norm_a -zero_a, ma[mid:]*scaling/norm_b -zero_b),axis=0)
#  atlas = np.concatenate((atlas[:mid]/norm_a -zero_a, atlas[mid:]*scaling/norm_b -zero_b),axis=0)
#else:
cbs = np.concatenate((cbs[:mid_1]/norm_a -zero_a, cbs[mid_1:mid_2]/norm_b -zero_b, cbs[mid_2:]/norm_c -zero_c),axis=0)
cm = np.concatenate((cm[:mid_1]/norm_a -zero_a, cm[mid_1:mid_2]/norm_b -zero_b, cm[mid_2:]/norm_c -zero_c),axis=0)
ma = np.concatenate((ma[:mid_1]/norm_a -zero_a, ma[mid_1:mid_2]/norm_b -zero_b, ma[mid_2:]/norm_c -zero_c),axis=0)
atlas = np.concatenate((atlas[:mid_1]/norm_a -zero_a, atlas[mid_1:mid_2]/norm_b -zero_b, atlas[mid_2:]/norm_c -zero_c),axis=0)

#ma = ma/norm -zero
print("CBS: ", cbs)
print("CM2: ", cm)
print("MA5: ", ma)


#if normalize != True:  # because otherwise you can't see the detail on the one that's 10x smaller
#  cbs_label = 'CBS scaled x' + str(scaling)
#  cm_label = 'CM2 scaled x' + str(scaling)
#  ma_label = 'MA5 scaled x' + str(scaling)
#  atlas_label = 'ATLAS scaled x' + str(scaling)
#else:
cbs_label = 'CBS'
cm_label = 'CM2'
ma_label = 'MA5'
atlas_label = 'ATLAS'
#if ma_tuned:
#  matune = matune/norm -zero
#atlas = atlas/norm -zero


#fig = plt.figure(figsize=(10,6))
fig, axs = plt.subplots(3, figsize=(10,6))

axs[0].errorbar(x, cbs, yerr=cbs_err, color=cmap(0), fmt='o', label=cbs_label)
#plt.errorbar(x, cbs, yerr=cbs_err, color=cmap(0), fmt='o', label=cbs_label)
axs[0].errorbar(x, cm, yerr=cm_err, color=cmap(1), fmt='x', label=cm_label)
#plt.errorbar(x, cm, yerr=cm_err, color=cmap(1), fmt='x', label=cm_label)
axs[0].errorbar(x, ma, yerr=ma_err, color=cmap(2), fmt='D', label=ma_label)
#plt.errorbar(x, ma, yerr=ma_err, color=cmap(2), fmt='D', label=ma_label)
axs[0].step(x, atlas, where='mid', label=atlas_label, color=cmap(3))
#plt.step(x, atlas, where='mid', label=atlas_label, color=cmap(3))


if ma_tuned:
  plt.errorbar(x, matune, yerr=matune_err, color=cmap(2.2), fmt='*', label='MA5tune')
#plt.step(x, atlas, where='mid', label='ATLAS', color=cmap(3))  # this was commented out when I got here

#if name == 'tab_16':  # the lengths of each SR... put them all in the same .txt file
#  SR1 = 8
#  SR2 = 13
#elif name == 'tab_17':
#  SR1 = 9
#  SR2 = 17
#else:
#  SR1 = 7
#  SR3 = 15
#  SR2 = 23

# plot ATLAS data for all signal regions
#plt.step(x, atlas, where='mid', label=atlas_label, color=cmap(3))
#plt.step(x[0:SR1], atlas[0:SR1], where='mid', label='ATLAS', color=cmap(3))
#if name == 'tab_18':
#  plt.step(x[SR1:SR3], atlas[SR1:SR3], where='mid', color=cmap(3))
#  plt.step(x[SR3:SR2], atlas[SR3:SR2], where='mid', color=cmap(3))
#else:
#  plt.step(x[SR1:SR2], atlas[SR1:SR2], where='mid', color=cmap(3))
#plt.step(x[SR2:], atlas[SR2:], where='mid', color=cmap(3))

if normalize:
  plt.legend(loc=4)
else:
  plt.legend(loc=1)



sr_name = [r"No cuts: $e^+e^-$",
r"Two signal leptons",
r"Jet veto",
r"Z veto",
r"SR-$m_{T2}^{90}$",
r"SR-$m_{T2}^{120}$",
r"SR-$m_{T2}^{150}$",
r"No cuts: $\mu^+\mu^-$",
r"Two signal leptons",
r"Jet veto",
r"Z veto",
r"SR-$m_{T2}^{90}$",
r"SR-$m_{T2}^{120}$",
r"SR-$m_{T2}^{150}$",
r"No cuts: $\mu^\pm e^\pm$",
r"Two signal leptons",
r"Jet veto",
r"Z veto",
r"SR-$m_{T2}^{90}$",
r"SR-$m_{T2}^{120}$",
r"SR-$m_{T2}^{150}$",
"test",
"test",
"test",
"test",
"test",
"test",
"test",
"test",
"test",
"test",
"test",
"test",
"test",
"test",
"test",
"test",
"test",
"test",
"test",
"test",
"test",
"test",
"test",
"test"]



axs[0].set_xticks(x)
axs[0].set_xticklabels(sr_name, rotation=90)

if normalize:
  plt.ylabel("Relative difference to ATLAS event yields")
  plt.yticks([-0.1,0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8],['-10%',0,'10%','20%','30%','40%','50%','60%','70%','80%'],rotation=90)
#  plt.yticks([-0.3,-0.2,-0.1,0,0.1,0.2,0.3],['-30%','-20%','-10%',0,'10%','20%','30%'],rotation=90)
  plt.ylim(-0.15,0.95)
#  if name == 'tab_18':
#    plt.ylim(-0.35,0.55)
    
else:
    plt.ylabel("Event yields")

plt.tight_layout()


name = "ATLAS-2013-11-pr4"

if normalize:
    plt.savefig('compare_norm_'+name+'.png',dpi=250)
else:
    plt.savefig('compare_'+name+'.png',dpi=250)

plt.suptitle("Cutflow comparison")
plt.show()
