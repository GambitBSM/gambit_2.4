 #!/usr/bin/env python
import os
import sys
from pylab import *
import pandas
import random
import matplotlib.colors as mcolors
cmap = plt.get_cmap("tab10")

print("~~~~~~~~~~~~~~~~~~~~~~~~~~")
print("REMEMBER TO PASS .txt FILES IN THIS ORDER: ee, mumu")
print("BTW the file path is hard-coded in so just the name is fine.")
print("~~~~~~~~~~~~~~~~~~~~~~~~~~\n")

xsect = 10.3
xsect_uncert = 0.0227*xsect 
evtcount = 50000.0

# name of file passed in
name_a = sys.argv[1]
name_b = sys.argv[2]
# whether or not to normalize
normalize = len(sys.argv)>3


# get data as 2d array of strings, with same rows & columns as .txt file
data_a = np.genfromtxt("data_2LEPEW/" + name_a,dtype="str")
data_b = np.genfromtxt("data_2LEPEW/" + name_b,dtype="str")
name_a = name_a[:-4]  # all except .txt
name_b = name_b[:-4]
# print(data_a)
# print(data_b)
data = np.concatenate((data_a, data_b), axis=0)
bad = data == 'no_data'
data[bad] = np.nan
#print(data)

x = range(len(data))
ma_tuned = data.shape[1] == 5

mid = 6  # row where the first file ends

atlas = data[:,0].astype(np.float)
atlas_a = atlas[:mid]
atlas_b = atlas[mid:]
#print(atlas_a)
#print(atlas_b)

if normalize:
    normalizer = atlas
    zero = 1
#    norm_a = atlas_a
#    norm_b = atlas_b
#    zero_a = 1
#    zero_b = 1
else:
    normalizer = 1
    zero = 0
#    norm_a = 1
#    norm_b = 1
#    zero_a = 0    
#    zero_b = 0    

cbs = data[:,3].astype(np.float)
cm = data[:,1].astype(np.float)
ma = data[:,2].astype(np.float)
#if ma_tuned:
#  matune = data[:,4].astype(np.float)
#if name == 'tab_16':  # I don't know how this works

factor = xsect*20.3/evtcount
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

 
#print(cbs)
#print(shape(cbs))
#print(len(cbs[:10]))
#print(len(cbs[10:]))
#print(len(norm_a))
#print(len(norm_b))

#scaling = 1.0/10  # because otherwise you can't see any detail on SRA
scaling = 1.0

#print(cbs)
#print(np.concatenate((cbs[:mid],cbs[mid:]),axis=0))
#print(normalizer)
#print(np.concatenate((norm_a,norm_b),axis=0) if normalize else "blah")

#cbs = np.concatenate((cbs[:mid]/norm_a -zero_a, cbs[mid:]/norm_b -zero_b),axis=0)
#cm = np.concatenate((cm[:mid]/norm_a -zero_a, cm[mid:]/norm_b -zero_b),axis=0)
#ma = np.concatenate((ma[:mid]/norm_a -zero_a, ma[mid:]/norm_b -zero_b),axis=0)
#atlas = np.concatenate((atlas[:mid]/norm_a -zero_a, atlas[mid:]/norm_b -zero_b),axis=0)
cbs = cbs/normalizer -zero 
cm = cm/normalizer -zero
ma = ma/normalizer -zero
atlas = atlas/normalizer -zero

#ma = ma/norm -zero
#print("CBS: ", cbs)
#print("CM2: ", cm)
#print("MA5: ", ma)


cbs_label = 'CBS'
cm_label = 'CM2'
ma_label = 'MA5'
atlas_label = 'ATLAS'


fig = plt.figure(figsize=(10,6))

plt.errorbar(x, cbs, yerr=cbs_err, color=cmap(0), fmt='o', label=cbs_label)
plt.errorbar(x, cm, yerr=cm_err, color=cmap(1), fmt='x', label=cm_label)
plt.errorbar(x, ma, yerr=ma_err, color=cmap(2), fmt='D', label=ma_label)
plt.step(x[0:mid], atlas[0:mid], where='mid', label='ATLAS', color=cmap(3))
plt.step(x[mid:], atlas[mid:], where='mid', color='green')

if ma_tuned:
  plt.errorbar(x, matune, yerr=matune_err, color=cmap(2.2), fmt='*', label='MA5tune')
#plt.step(x, atlas, where='mid', label='ATLAS', color=cmap(3))  # this was commented out when I got here

if normalize:
  plt.legend(loc=1)
else:
  plt.legend(loc=1)


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




plt.xticks(x,sr_name,rotation=90)

if normalize:
  plt.ylabel("Relative difference to ATLAS event yields")
#  plt.yticks([-0.3,-0.2,-0.1,0,0.1,0.2,0.3],['-30%','-20%','-10%',0,'10%','20%','30%'],rotation=90)
  plt.yticks([-1.0,-0.8,-0.6,-0.4,-0.2,0],['-100%','-80%','-60%','-40%','-20%',0],rotation=90)
  plt.ylim(-1.25,0.05)
    
else:
    plt.ylabel("Event yields")

plt.tight_layout()


name = "ATLAS-2013-11-pr1"

if normalize:
    plt.savefig('compare_norm_'+name+'.png',dpi=250)
else:
    plt.savefig('compare_'+name+'.png',dpi=250)

plt.title("Cutflow comparison")
plt.show()
