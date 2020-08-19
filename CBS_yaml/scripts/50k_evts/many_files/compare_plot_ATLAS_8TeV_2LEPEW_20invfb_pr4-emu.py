 #!/usr/bin/env python
import os
import sys
from pylab import *
import pandas
import random
import matplotlib.colors as mcolors
cmap = plt.get_cmap("tab10")

print("~~~~~~~~~~~~~~~~~~~~~~~~~~")
print("REMEMBER TO PASS .txt FILES IN THIS ORDER: SR1, SR2, SR3")
print("BTW the file path is hard-coded in so just the name is fine.")
print("~~~~~~~~~~~~~~~~~~~~~~~~~~\n")    

xsect = 380
xsect_uncert = 0.037*xsect
evtcount = 50000

# name of file passed in
name_a = sys.argv[1]
name_b = sys.argv[2]
name_c = sys.argv[3]
# whether or not to normalize
normalize = len(sys.argv)>4


# get data as 2d array of strings, with same rows & columns as .txt file
data_a = np.genfromtxt("data_2LEPEW/" +name_a,dtype="str")
data_b = np.genfromtxt("data_2LEPEW/" +name_b,dtype="str")
data_c = np.genfromtxt("data_2LEPEW/" +name_c,dtype="str")
name_a = name_a[:-4]  # all except .txt
name_b = name_b[:-4]
name_c = name_c[:-4]
print('~~~~~~~~~~~~~~~~~~~~~~~~')
print(name_a[-3:])
if ((name_a[-3:]!='emu') or (name_b[-3:]!='emu') or (name_c[-3:]!='emu')) and (name_a[-6:]!='BEFORE'):
  raise Exception("Sorry, you need to input -emu.txt files")

# print(data_a)
# print(data_b)
data = np.concatenate((data_a, data_b, data_c), axis=0)
bad = data == 'no_data'
data[bad] = np.nan
#print(data)


x = range(len(data))
ma_tuned = data.shape[1] == 5

mid_1 = 6  # row where the first file ends
mid_2 = 11  # row where the 2nd file ends

atlas = data[:,0].astype(np.float)
atlas_a = atlas[:mid_1]
atlas_b = atlas[mid_1:mid_2]
atlas_c = atlas[mid_2:]
#print(atlas_a)
#print(atlas_b)
#print(atlas_c)

if normalize:
    norm = atlas
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

factor = xsect*20.3/evtcount
factor_cbs = xsect*20.3/242500  # for some reason this was the initial evt count for CBS for LEPEW analysis
if normalize:
  cbs_err = np.sqrt(cbs*factor_cbs)/norm
  cm_err = np.sqrt(cm*factor)/norm
  ma_err = np.sqrt(ma*factor)/norm
else:
  cbs_err = np.sqrt(cbs*factor_cbs)
  cm_err = np.sqrt(cm*factor)
  ma_err = np.sqrt(ma*factor)

#print(cbs)
#print(shape(cbs))
#print(len(cbs[:10]))
#print(len(cbs[10:]))
#print(len(norm_a))
#print(len(norm_b))

#scaling = 1.0/10  # because otherwise you can't see any detail on SRA
scaling = 1.0

cbs = np.concatenate((cbs[:mid_1]/norm_a -zero_a, cbs[mid_1:mid_2]/norm_b -zero_b, cbs[mid_2:]/norm_c -zero_c),axis=0)
cm = np.concatenate((cm[:mid_1]/norm_a -zero_a, cm[mid_1:mid_2]/norm_b -zero_b, cm[mid_2:]/norm_c -zero_c),axis=0)
ma = np.concatenate((ma[:mid_1]/norm_a -zero_a, ma[mid_1:mid_2]/norm_b -zero_b, ma[mid_2:]/norm_c -zero_c),axis=0)
atlas = np.concatenate((atlas[:mid_1]/norm_a -zero_a, atlas[mid_1:mid_2]/norm_b -zero_b, atlas[mid_2:]/norm_c -zero_c),axis=0)

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
plt.step(x, atlas, where='mid', label=atlas_label, color='blue')


if ma_tuned:
  plt.errorbar(x, matune, yerr=matune_err, color=cmap(2.2), fmt='*', label='MA5tune')


if normalize:
  plt.legend(loc=4)
else:
  plt.legend(loc=1)



sr_name = [r"Two signal $e^\pm\mu^\mp$",
r"Jet veto",
r"Z veto",
r"$p_{T,ll}>80$ GeV",
r"$E_T^{miss,rel}>80$ GeV",
r"$m_{ll}<170$ GeV",
r"Two signal $e^\pm\mu^\mp$",
r"Jet veto",
r"Z veto",
r"$m_{T2}>90$ GeV",
r"$m_{ll}>170$ GeV",
r"Two signal $e^\pm\mu^\mp$",
r"Jet veto",
r"Z veto",
r"$m_{T2}>100$ GeV"]



plt.xticks(x, sr_name, rotation=90)

if normalize:
  plt.ylabel("Relative difference to ATLAS event yields")
#  plt.yticks([-0.3,0,0.3],['-30%',0,'30%'],rotation=90)
#  plt.yticks([-0.9,-0.6,-0.3,0,0.3,0.6,0.9,1.2, 2.0],['-90%','-60%','-30%',0,'30%','60%','90%','120%','200%'],rotation=90)
  plt.yticks([-0.9,-0.6,-0.3,0,0.3,0.6,0.9],['-90%','-60%','-30%',0,'30%','60%','90%'],rotation=90)
  plt.ylim(-1.05,1.05)
else:
  plt.ylabel("Event yields")

plt.tight_layout()


name = "ATLAS-2013-11-pr4-emu"

if normalize:
    plt.savefig('compare_norm_'+name+'.png',dpi=250)
else:
    plt.savefig('compare_'+name+'.png',dpi=250)

plt.title("Cutflow comparison")
plt.show()
