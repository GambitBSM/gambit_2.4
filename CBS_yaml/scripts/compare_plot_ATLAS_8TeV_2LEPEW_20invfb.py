 #!/usr/bin/env python
import os
import sys
from pylab import *
import pandas
import random
import matplotlib.colors as mcolors
cmap = plt.get_cmap("tab10")

# Plotting script for cutflows generated by enabling cutflow output in Analysis_ATLAS_8TeV_2LEPEW_20invfb.cpp
# Code by Kelton Whiteaker

print("\n~~~~~~~~~~~~~~")
print("Pass in 1 or 2 args: (1) the file containing data to be plotted, (2) any 2nd argument if you wanna normalize wrt ATLAS")
print("The script will figure out which process you're doing from the name of the .txt file. For process 4, this file must be either all ee, mumu or emu.")
print("The file path (data_2LEPEW/.) is hard-coded in so just enter the name.")
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
  if 'ee' in name:
    pr4_SR = 'ee'
  if 'mumu' in name:
    pr4_SR = 'mumu'
  if 'emu' in name:
    pr4_SR = 'emu'

print("You are doing: " + name[:-4])
if process == 4:
  print("Process: " + str(process))
  print("Pr4 SR: " + pr4_SR + '\n')
else:
  print("Process: " + str(process)+'\n')

# get data as 2d array of strings, with same rows & columns as .txt file
data = np.genfromtxt("data_2LEPEW/" + name,dtype="str", skip_header=1)
name = name[:-4]  # all except .txt
bad = data == 'no_data'
data[bad] = np.nan

evtcount = 400000  # number of initially generated events
evtcount_cbs = evtcount  # sometimes it's different
#if process == 4:  # in case evtcount is different for CBS for certain processes
#  evtcount_cbs = 800000
#else:
#  evtcount_cbs = 250000
num_SRs = 2 if (process == 1 or process == 3) else 3
if process == 1:
  xsect = 10.3  #fb
  xsect_uncert = 0.0227*xsect
  SRmumu = 6  # number of data points until the "mumu" version of the cutflow
elif process == 2:
  xsect = 13.60409
  xsect_uncert = 0.1744570*xsect
  SRmumu = 6
  SRemu = 12
elif process == 3:
  xsect = 70.69 
  xsect_uncert = 0.05439*xsect
  SRmumu = 9
elif process == 4:
  xsect_1 = 5550.178
  xsect_uncert_1 = 0.064931*xsect_1
  xsect_2 = 1530
  xsect_uncert_2 = 0.0522878*xsect_2
  xsect_3 = 380
  xsect_uncert_3 = 0.037*xsect_3
  SR2 = 6  # number of data points until pr4's 2nd SR
  SR3 = 11  # same but for its 3rd SR

x = range(len(data))
ma_tuned = data.shape[1] == 5  # this notes whether you have a second ma5 column (which we will, someday)
# I don't really have support for a 5th column for this analysis. I do for the 2bStop analysis.

atlas = data[:,0].astype(np.float)
if process == 4:
  atlas_1 = atlas[:SR2]
  atlas_2 = atlas[SR2:SR3]
  atlas_3 = atlas[SR3:]
else:
  atlas_ee = atlas[:SRmumu]
  atlas_mumu = atlas[SRmumu:SRemu] if num_SRs == 3 else atlas[SRmumu:]
  atlas_emu = atlas[SRemu:] if num_SRs == 3 else atlas[:]
cbs = data[:,3].astype(np.float)
cm = data[:,1].astype(np.float)
ma = data[:,2].astype(np.float)
if ma_tuned:
  matune = data[:,4].astype(np.float)

if normalize:
  if process == 4:
    norm_a = atlas_1
    norm_b = atlas_2
    norm_c = atlas_3
  else:
    normalizer = atlas
  zero = 1
else:
  if process == 4:
    norm_a = 1 
    norm_b = 1 
    norm_c = 1
  else:
    normalizer = 1
  zero = 0    

if process != 4:
  factor = xsect*20.3/evtcount  # same xsect for all SRs (ee/mumu/emu because same process... this doesn't apply to pr4!)
  factor_cbs = xsect*20.3/evtcount_cbs
  if normalize:
    cbs_err = np.sqrt(cbs*factor_cbs)/normalizer
    cm_err = np.sqrt(cm*factor)/normalizer
    ma_err = np.sqrt(ma*factor)/normalizer
  else:
    cbs_err = np.sqrt(cbs*factor_cbs)
    cm_err = np.sqrt(cm*factor)
    ma_err = np.sqrt(ma*factor)
elif process == 4:
  factor1 = xsect_1*20.3/evtcount
  factor2 = xsect_2*20.3/evtcount
  factor3 = xsect_3*20.3/evtcount
  factor1_cbs = xsect_1*20.3/evtcount_cbs 
  factor2_cbs = xsect_2*20.3/evtcount_cbs 
  factor3_cbs = xsect_3*20.3/evtcount_cbs 
  if normalize:
    cbs_err = np.concatenate((np.sqrt(cbs[:SR2]*factor1_cbs)/norm_a, np.sqrt(cbs[SR2:SR3]*factor2_cbs)/norm_b, np.sqrt(cbs[SR3:]*factor3_cbs)/norm_c), axis=0)
    cm_err = np.concatenate((np.sqrt(cm[:SR2]*factor1)/norm_a, np.sqrt(cm[SR2:SR3]*factor2)/norm_b, np.sqrt(cm[SR3:]*factor3)/norm_c), axis=0)
    ma_err = np.concatenate((np.sqrt(ma[:SR2]*factor1)/norm_a, np.sqrt(ma[SR2:SR3]*factor2)/norm_b, np.sqrt(ma[SR3:]*factor3)/norm_c), axis=0)
  else:
    cbs_err = np.concatenate((np.sqrt(cbs[:SR2]*factor1_cbs), np.sqrt(cbs[SR2:SR3]*factor2_cbs), np.sqrt(cbs[SR3:]*factor3_cbs)), axis=0)
    cm_err = np.concatenate((np.sqrt(cm[:SR2]*factor1), np.sqrt(cm[SR2:SR3]*factor2), np.sqrt(cm[SR3:]*factor3)), axis=0)
    ma_err = np.concatenate((np.sqrt(ma[:SR2]*factor1), np.sqrt(ma[SR2:SR3]*factor2), np.sqrt(ma[SR3:]*factor3)), axis=0)

if process == 4:
  cbs = np.concatenate((cbs[:SR2]/norm_a -zero, cbs[SR2:SR3]/norm_b -zero, cbs[SR3:]/norm_c -zero),axis=0)
  cm = np.concatenate((cm[:SR2]/norm_a -zero, cm[SR2:SR3]/norm_b -zero, cm[SR3:]/norm_c -zero),axis=0)
  ma = np.concatenate((ma[:SR2]/norm_a -zero, ma[SR2:SR3]/norm_b -zero, ma[SR3:]/norm_c -zero),axis=0)
  atlas = np.concatenate((atlas[:SR2]/norm_a -zero, atlas[SR2:SR3]/norm_b -zero, atlas[SR3:]/norm_c -zero),axis=0)
else:
  cbs = cbs/normalizer -zero
  cm = cm/normalizer -zero
  ma = ma/normalizer -zero
  atlas = atlas/normalizer -zero

#print("CBS: ", cbs)  # for debugging
#print("CM2: ", cm)
#print("MA5: ", ma)


# ~~~~~~~~~~ plotting ~~~~~~~~~~

fig = plt.figure(figsize=(10,6))

cbs_label = 'CBS'
cm_label = 'CM2'
ma_label = 'MA5'
atlas_label = 'ATLAS'

plt.errorbar(x, cm, yerr=cm_err, color=cmap(1), fmt='x', label=cm_label)
plt.errorbar(x, ma, yerr=ma_err, color=cmap(2), fmt='D', label=ma_label)
plt.errorbar(x, cbs, yerr=cbs_err, color=cmap(0), fmt='o', label=cbs_label)
# in the following, I split ATLAS into different sections so it has a line break on the plot
if process == 4:
  if pr4_SR == 'ee':
    plt.step(x[:SR2], atlas[:SR2], where='mid', label=atlas_label, color=cmap(3))
    plt.step(x[SR2:SR3], atlas[SR2:SR3], where='mid', color=cmap(3))
    plt.step(x[SR3:], atlas[SR3:], where='mid', color=cmap(3))
  elif pr4_SR == 'mumu':
    plt.step(x[:SR2], atlas[:SR2], where='mid', label=atlas_label, color='green')
    plt.step(x[SR2:SR3], atlas[SR2:SR3], where='mid', color='green')
    plt.step(x[SR3:], atlas[SR3:], where='mid', color='green')
  elif pr4_SR == 'emu':
    plt.step(x[:SR2], atlas[:SR2], where='mid', label=atlas_label, color='blue')
    plt.step(x[SR2:SR3], atlas[SR2:SR3], where='mid', color='blue')
    plt.step(x[SR3:], atlas[SR3:], where='mid', color='blue')
else:
  plt.step(x[0:SRmumu], atlas[0:SRmumu], where='mid', label=atlas_label, color=cmap(3))
  if num_SRs == 2:
    plt.step(x[SRmumu:], atlas[SRmumu:], where='mid', color='green')
  elif num_SRs == 3:
    plt.step(x[SRmumu:SRemu], atlas[SRmumu:SRemu], where='mid', color='green')
    plt.step(x[SRemu:], atlas[SRemu:], where='mid', color='blue')


#if ma_tuned:
#  plt.errorbar(x, matune, yerr=matune_err, color=cmap(2.2), fmt='*', label='MA5tune')

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
elif process == 4:
  if pr4_SR == 'ee':
    sr_name_init = [r"Two signal $e^+e^-$"]
  elif pr4_SR == 'mumu':
    sr_name_init = [r"Two signal $\mu^+\mu^-$"]
  elif pr4_SR == 'emu':
    sr_name_init = [r"Two signal $e^+\mu^-$"]
  sr_name_1 = [r"Jet veto",
  r"Z veto",
  r"$p_{T,ll}>80$ GeV",
  r"$E_T^{miss,rel}>80$ GeV",
  r"$m_{ll}<170$ GeV"]
  sr_name_2 = [r"Jet veto",
  r"Z veto",
  r"$m_{T2}>90$ GeV",
  r"$m_{ll}>170$ GeV"]
  sr_name_3 = [r"Jet veto",
  r"Z veto",
  r"$m_{T2}>100$ GeV"]
  sr_name = sr_name_init + sr_name_1 + sr_name_init + sr_name_2 + sr_name_init + sr_name_3

plt.xticks(x,sr_name,rotation=90)

if process == 1:
  if normalize:
    plt.ylabel("Relative difference to ATLAS event yields")
  #  plt.yticks([-0.3,-0.2,-0.1,0,0.1,0.2,0.3],['-30%','-20%','-10%',0,'10%','20%','30%'],rotation=90)
    plt.yticks([-0.8,-0.6,-0.4,-0.2,0,0.2,0.4],['-80%','-60%','-40%','-20%',0,'20%','40%'],rotation=90)
    plt.ylim(-1.05,0.55)
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
    plt.ylim(-0.75,0.55)
  else:
    plt.ylabel("Event yields")
elif process == 4:
  if pr4_SR == 'ee':
    if normalize:
        plt.ylabel("Relative difference to ATLAS event yields")
        plt.yticks([-0.6,-0.4,-0.2,0,0.2,0.4,0.6],['-60%','-40%','-20%',0,'20%','40%','60%'],rotation=90)
      #  plt.yticks([-0.3,-0.2,-0.1,0,0.1,0.2,0.3],['-30%','-20%','-10%',0,'10%','20%','30%'],rotation=90)
        plt.ylim(-0.45,0.55)
    else:
      plt.ylabel("Event yields")            
  if pr4_SR == 'mumu':
    if normalize:
      plt.ylabel("Relative difference to ATLAS event yields")
    #  plt.yticks([-0.9,-0.6,-0.3,0,0.3,0.6,0.9,1.2,1.4],['-90%','-60%','-30%',0,'30%','60%','90%','120%','150%'],rotation=90)
      plt.yticks([-0.4,-0.2,0,0.2,0.4,0.6],['-40%','-20%',0,'20%','40%','60%'],rotation=90)
      plt.ylim(-0.45,0.65)
    else:
      plt.ylabel("Event yields")            
  if pr4_SR == 'emu':
    if normalize:
      plt.legend(loc=4)
      plt.ylabel("Relative difference to ATLAS event yields")
    #  plt.yticks([-0.9,-0.6,-0.3,0,0.3,0.6,0.9,1.2, 2.0],['-90%','-60%','-30%',0,'30%','60%','90%','120%','200%'],rotation=90)
      plt.yticks([-0.6,-0.4,-0.2,0,0.2,0.4,0.6,0.8],['-60%','-40%','-20%',0,'20%','40%','60%','80%'],rotation=90)
      plt.ylim(-0.35,0.95)
    else:
      plt.ylabel("Event yields")            


plt.tight_layout()

save_name = name  # I'm keeping it simple lol but you can append "ATLAS-" or something if you want

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
